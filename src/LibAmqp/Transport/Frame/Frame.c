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

#include "debug_helper.h"

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
        amqp_error(context, AMQP_ERROR_INVALID_FRAME_TYPE, "Invalid frame type: %02x", frame->frame_type);
        return false;
    }

    return true;
}

void amqp_symbol_initialize_from_type(amqp_symbol_t *symbol, amqp_buffer_t *buffer, amqp_type_t *type)
{
    // TODO - deal with case where symbol straddles buffer fragments - not an issue for performative frames as they should fit in the smallest buffer.
    amqp_symbol_initialize_reference(symbol, buffer, amqp_buffer_pointer(buffer, type->position.index), type->position.size);
}

static int decode_symbolic_descriptor(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *type)
{
    amqp_symbol_t symbol;

    amqp_descriptor_t *descriptor;
    amqp_symbol_initialize_from_type(&symbol, buffer, type);
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


    code = amqp_type_to_ulong(type);
    if (amqp_type_is_convert_failed(type))
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Descriptor is not a symbol or ulong.");
        // TODO - dump dodgy descriptor
        return false;
    }

    frame->descriptor.domain = code >> 32;
    frame->descriptor.id = (uint32_t) code;
    frame->descriptor.group = 0;

    return true;
}

static int decode_sasl_mechanisms_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *type)
{
    if (frame->frame_type != AMQP_SASL_FRAME_TYPE)
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Expected a SASL frame type.");
        // TODO - dump type
        return false;
    }

    not_implemented(todo);
}

static int decode_remainder(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *type)
{
    // Currently all accepted frames are encoded as a list.
    if (!amqp_type_is_list(type))
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Expected a list.");
        // TODO - dump type
        return false;
    }

    if (frame->descriptor.domain != AMQP_DESCRIPTOR_DOMAIN)
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Unknow descriptor domain. Domain = %d", frame->descriptor.domain);
        // TODO - dump type
        return false;
    }

    // Check that the frame type match's the frame id.
    switch (frame->descriptor.id)
    {
        case AMQP_FRAME_ID_SASL_MECHANISMS_LIST:
            return decode_sasl_mechanisms_frame(context, buffer, frame, type);

        case AMQP_FRAME_ID_OPEN_LIST:
        case AMQP_FRAME_ID_BEGIN_LIST:
        case AMQP_FRAME_ID_ATTACH_LIST:
        case AMQP_FRAME_ID_FLOW_LIST:
        case AMQP_FRAME_ID_TRANSFER_LIST:
        case AMQP_FRAME_ID_DISPOSITION_LIST:
        case AMQP_FRAME_ID_DETACH_LIST:
        case AMQP_FRAME_ID_END_LIST:
        case AMQP_FRAME_ID_CLOSE_LIST:
        case AMQP_FRAME_ID_ERROR_LIST:
        case AMQP_FRAME_ID_SASL_INIT_LIST:
        case AMQP_FRAME_ID_SASL_CHALLENGE_LIST:
        case AMQP_FRAME_ID_SASL_RESPONSE_LIST:
        case AMQP_FRAME_ID_SASL_OUTCOME_LIST:

        default:
            amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Unsupported descriptor id. Id = %08x", frame->descriptor.id);
            // TODO - dump type
            return false;
    }
}

static int decode_performative(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *type)
{
    if (!amqp_type_is_valid(type))
    {
        amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode frame. Frame is not a valid AMQP type");
        return false;
    }

    return decode_descriptor(context, buffer, frame, amqp_type_get_descriptor(type)) &&
            decode_remainder(context, buffer, frame, amqp_type_get_described(type));
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

    rc = decode_performative(context, buffer, frame, type);
    amqp_deallocate_type(context, type);
    return rc;
}

amqp_frame_t *amqp_decode_frame(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_frame_t *result = amqp_allocate_frame(context);

    if (decode_frame(context, buffer, result) == 0)
    {
        amqp_deallocate_frame(context, result);
        return 0;
    }

    return result;
}
