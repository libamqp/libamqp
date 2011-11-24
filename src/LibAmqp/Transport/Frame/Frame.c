/*
   Copyright 2011 StormMQ Limited

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#include "Context/Context.h"
#include "Transport/Frame/Frame.h"
#include "Codec/Decode/Decode.h"
#include "Codec/Type/TypePrint.h"
#include "AmqpTypes/AmqpTypes.h"
#include "Transport/Connection/FrameDispatch.h"

#include "debug_helper.h"

#include "AmqpTypes/Decode/AmqpTypesDecode.h"
#include "AmqpTypes/Decode/AmqpDefinitionsDecode.h"

#include "Transport/Decode/DecodeSaslFrame.h"
#include "Transport/Decode/DecodeTransportFrame.h"

static int decode_header(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame)
{
    assert(amqp_buffer_size(buffer) >= AMQP_FRAME_HEADER_SIZE);

    frame->data_offset = ((uint32_t) amqp_buffer_read_uint8(buffer, 4)) * AMQP_FRAME_HEADER_DATA_OFFSET_MULTIPLIER;
    frame->frame_type = amqp_buffer_read_uint8(buffer, 5);
    frame->type_specific.word = amqp_buffer_read_uint16(buffer, 6);

    amqp_buffer_advance_read_index(buffer, frame->data_offset);

    if (frame->data_offset < AMQP_FRAME_HEADER_SIZE)
    {
        amqp_error(context, AMQP_ERROR_CORRUPT_FRAME_HEADER, "Invalid data offset: %u", frame->data_offset);
        return false;
    }

    if  (frame->frame_type != AMQP_FRAME_TYPE && frame->frame_type != AMQP_SASL_FRAME_TYPE)
    {
        amqp_error(context, AMQP_ERROR_INVALID_FRAME_TYPE, "Unsupported frame type: %02x", frame->frame_type);
        return false;
    }

    return true;
}

static int decode_symbolic_descriptor(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *type)
{
    amqp_symbol_t symbol;

    amqp_descriptor_t *descriptor;
    amqp_symbol_initialize_from_type(context, &symbol, type);
    descriptor = amqp_context_descriptor_lookup(context, &symbol);
    amqp_symbol_cleanup(context, &symbol);

    if (descriptor)
    {
        frame->descriptor.domain = descriptor->domain;
        frame->descriptor.id = descriptor->id;
        frame->descriptor.group = descriptor->group;
        return true;
    }

    amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Descriptor unknown.");
    // TODO - dump dodgy descriptor
    return false;
}

static int decode_descriptor(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *type)
{
    uint64_t code;

    if (amqp_type_is_symbol(type))
    {
        return decode_symbolic_descriptor(context, buffer, frame, type);
    }

    if (!amqp_type_is_ulong(type))
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Descriptor is not a symbol or ulong.");
        return false;
    }
    code = amqp_type_to_ulong(type);

    if (frame->descriptor.domain != AMQP_DESCRIPTOR_DOMAIN)
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Frame is for an unsupported descriptor domain. Domain = %d", frame->descriptor.domain);
        return false;
    }

    frame->descriptor.domain = code >> 32;
    frame->descriptor.id = (uint32_t) code;
    frame->descriptor.group = 0;

    return true;
}

static int decode_remainder(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *descriptor_type, amqp_type_t *type)
{

    // Currently all accepted frames are encoded as a list.
    if (!amqp_type_is_list(type))
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Expected an AMQP list type.");
        amqp_type_print(context, type);
        return false;
    }

    // Check that the frame type match's the frame id.
    switch (frame->descriptor.id)
    {
        case amqp_sasl_mechanisms_list_descriptor:
            return amqp_decode_sasl_mechanisms_frame(context, buffer, frame, type);

        case amqp_sasl_init_list_descriptor:
            return amqp_decode_sasl_init_frame(context, buffer, frame, type);

        case amqp_sasl_challenge_list_descriptor:
            return amqp_decode_sasl_challenge_frame(context, buffer, frame, type);

        case amqp_sasl_response_list_descriptor:
            return amqp_decode_sasl_response_frame(context, buffer, frame, type);

        case amqp_sasl_outcome_list_descriptor:
            return amqp_decode_sasl_outcome_frame(context, buffer, frame, type);

        case amqp_open_list_descriptor:
            return amqp_decode_amqp_open_frame(context, buffer, frame, type);

        case amqp_begin_list_descriptor:
            return amqp_decode_amqp_begin_frame(context, buffer, frame, type);

        case amqp_attach_list_descriptor:
            return amqp_decode_amqp_attach_frame(context, buffer, frame, type);

        case amqp_flow_list_descriptor:
            return amqp_decode_amqp_flow_frame(context, buffer, frame, type);

        case amqp_transfer_list_descriptor:
            return amqp_decode_amqp_transfer_frame(context, buffer, frame, type);

        case amqp_disposition_list_descriptor:
            return amqp_decode_amqp_disposition_frame(context, buffer, frame, type);

        case amqp_detach_list_descriptor:
            return amqp_decode_amqp_detach_frame(context, buffer, frame, type);

        case amqp_end_list_descriptor:
            return amqp_decode_amqp_end_frame(context, buffer, frame, type);

        case amqp_close_list_descriptor:
            return amqp_decode_amqp_close_frame(context, buffer, frame, type);

        default:
            amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Unsupported descriptor id. Id = %08x", frame->descriptor.id);
            frame->descriptor.id = 0;   // Cleanup will just deallocate frame, not it's contents
            return false;
    }
}

static int decode_performative(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *type)
{
    int rc;

    if (!amqp_type_is_valid(type))
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Frame is not a valid AMQP type");
        // TODO - dump frame buffer
        return false;
    }

    rc = decode_descriptor(context, buffer, frame, amqp_type_get_descriptor(type));
    if (rc == 0)
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame descriptor");
    }

    if (rc)
    {
        rc = decode_remainder(context, buffer, frame, amqp_type_get_descriptor(type), amqp_type_get_described(type));
    }

    if (rc == 0)
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Dumping type...");
        amqp_type_dump(context, 2, type);
    }

    return rc;
}

static int decode_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame)
{
    amqp_type_t *type;
    int rc;

    if (decode_header(context, buffer, frame) == 0)
    {
        return false;
    }

    if ((type = amqp_decode(context, buffer)) == 0)
    {
        frame->descriptor.group = amqp_empty_frame;
        return true;
    }

    frame->type = type;
    rc = decode_performative(context, buffer, frame, type);
//    amqp_deallocate_type(context, type);
    return rc;
}

static amqp_frame_t *amqp_decode_frame(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_frame_t *result = amqp_allocate_frame(context);

    if (decode_frame(context, buffer, result) == 0)
    {
        amqp_frame_cleanup(context, result);
        return 0;
    }

    return result;
}

amqp_frame_t *amqp_decode_sasl_frame(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_frame_t *result = amqp_decode_frame(context, buffer);

    if (result && result->descriptor.id == 0)
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Received an empty frame when a SASL frame was expected.");
        amqp_frame_cleanup(context, result);
        return 0;
    }

    if (result && result->frame_type != AMQP_SASL_FRAME_TYPE)
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Expected a SASL frame. Decoded: %s", amqp_descriptor_id_to_cstr(result->descriptor.id));
        amqp_frame_cleanup(context, result);
        return 0;
    }

    return result;
}

amqp_frame_t *amqp_decode_amqp_frame(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_frame_t *result = amqp_decode_frame(context, buffer);

    if (result && result->descriptor.id && result->frame_type != AMQP_FRAME_TYPE)
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Expected an AMQP frame. Decoded: %s", amqp_descriptor_id_to_cstr(result->descriptor.id));
        amqp_frame_cleanup(context, result);
        return 0;
    }

    return result;
}

void amqp_frame_cleanup(amqp_context_t *context, amqp_frame_t *frame)
{
    if (frame == 0)
    {
        return;
    }

    if (frame->type)
    {
        amqp_deallocate_type(context, frame->type);
    }

    if (frame->descriptor.id != 0)
    {
        if (frame->cleanup)
        {
            frame->cleanup(context, frame);
        }
        else
        {
            amqp_error(context, AMQP_ERROR_ILLEGAL_STATE, "Frame does not have a cleanup method. Id: %08x", frame->descriptor.id);
        }
    }

    amqp_deallocate_frame(context, frame);
}

void amqp_frame_dump(amqp_context_t *context, amqp_frame_t *frame)
{
    amqp_context_printf(context, 2, "Frame %08x:%08x - ", frame->descriptor.domain, frame->descriptor.id);
    amqp_type_dump(context, 2, frame->type);
    amqp_context_putc(context, '\n');
}

uint64_t amqp_frame_descriptor_full_id(amqp_frame_t *frame)
{
    if (frame == 0)
    {
        return -1;
    }
    return (uint64_t) frame->descriptor.domain << 32 | frame->descriptor.id;
}

