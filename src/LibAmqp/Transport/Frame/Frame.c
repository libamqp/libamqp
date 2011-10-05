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

static int decode_performative(amqp_context_t *context, amqp_frame_t *frame, amqp_type_t *type)
{
    amqp_type_t *descriptor = amqp_type_get_descriptor(type);
    amqp_type_t *described = amqp_type_get_described(type);

    assert(descriptor && described);

    return true;
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
        frame->selector = amqp_empty_frame;
        return true;
    }

    rc = decode_performative(context, frame, type);
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