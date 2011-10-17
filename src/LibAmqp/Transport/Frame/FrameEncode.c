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
#include "Codec/Encode/Encode.h"
#include "AmqpTypes/AmqpTypes.h"
#include "Transport/Sasl/SaslMechanisms.h"

#include "debug_helper.h"

//static uint8_t sasl_frame_header[] = { 0, 0, 0, 0, 0, AMQP_SASL_FRAME_TYPE, 0, 0 };
//static uint8_t amqp_frame_header[] = { 0, 0, 0, 0, 0, AMQP_FRAME_TYPE, 0, 0 };
//
//static int decode_header(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame)
//{
//    assert(amqp_buffer_size(buffer) >= AMQP_FRAME_HEADER_SIZE);
//
//    frame->data_offset = ((uint32_t) amqp_buffer_read_uint8(buffer, 4)) * AMQP_FRAME_HEADER_DATA_OFFSET_MULTIPLIER;
//    frame->frame_type = amqp_buffer_read_uint8(buffer, 5);
//    frame->type_specific.word = amqp_buffer_read_uint16(buffer, 6);
//
//    amqp_buffer_advance_read_index(buffer, frame->data_offset);
//
//    if (frame->data_offset < AMQP_FRAME_HEADER_SIZE)
//    {
//        amqp_error(context, AMQP_ERROR_CORRUPT_FRAME_HEADER, "Invalid data offset: %u", frame->data_offset);
//        return false;
//    }
//
//    if  (frame->frame_type != AMQP_FRAME_TYPE && frame->frame_type != AMQP_SASL_FRAME_TYPE)
//    {
//        amqp_error(context, AMQP_ERROR_INVALID_FRAME_TYPE, "Unsupported frame type: %02x", frame->frame_type);
//        return false;
//    }
//
//    return true;
//}


void amqp_encode_sasl_frame(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_type_t *frame, *list;
    size_t performativce_offset;
    size_t frame_size;;
    uint16_t frame_type_specific = 0;
    
    amqp_buffer_advance_write_point(buffer, 8);

    performativce_offset = amqp_buffer_write_point(buffer);

    frame = amqp_start_encode_described_type(context, buffer);

    amqp_encode_ulong(context, buffer, AMQP_FRAME_ID_SASL_MECHANISMS_LIST);
    list = amqp_encode_list_8(context, buffer);

amqp_sasl_mechanisms_encode(context, buffer);

    amqp_complete_type(context, buffer, list);
    amqp_complete_type(context, buffer, frame);
    frame_size = amqp_buffer_size(buffer);
    amqp_buffer_set_write_index(buffer, 0);
    amqp_buffer_write_uint32(buffer, frame_size);
    amqp_buffer_write_uint8(buffer, performativce_offset / AMQP_FRAME_HEADER_DATA_OFFSET_MULTIPLIER);
    amqp_buffer_write_uint8(buffer, AMQP_SASL_FRAME_TYPE);
    amqp_buffer_write_uint16(buffer, frame_type_specific);
    amqp_buffer_set_write_index(buffer, frame_size);
    amqp_deallocate_type(context, frame);
}
