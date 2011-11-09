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

#include <TestHarness.h>
#include "Transport/Frame/Frame.h"
#include "Transport/Frame/FrameTestSupport.h"

#include "Codec/Decode/Decode.h"
#include "Codec/Type/TypePrint.h"
#include "debug_helper.h"

SUITE(Frame)
{
    TEST_FIXTURE(FrameFixture, client_open_frame)
    {
        test_data::client_open_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_open_t *open = &frame->frames.amqp.open;

        CHECK(amqp_string_compare_with_cstr(&open->container_id, "localhost.unknown-process-name") == 0);
        CHECK(amqp_string_compare_with_cstr(&open->hostname, "development.mysystem.mycompany.stormmq.com") == 0);

        CHECK_EQUAL(512U, open->max_frame_size);
        CHECK_EQUAL(255U, open->channel_max);
        CHECK_EQUAL(15000U, open->idle_time_out);

        CHECK(amqp_multiple_symbol_is_null(&open->outgoing_locales));
        CHECK(amqp_multiple_symbol_is_null(&open->incoming_locales));
        CHECK(amqp_multiple_symbol_is_null(&open->offered_capabilities));
        CHECK(amqp_multiple_symbol_is_null(&open->desired_capabilities));

        CHECK(amqp_map_is_null_or_empty(&open->properties));
    }

    TEST_FIXTURE(FrameFixture, broker_open_frame)
    {
        test_data::broker_open_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_open_t *open = &frame->frames.amqp.open;

        CHECK(amqp_string_compare_with_cstr(&open->container_id, "/mycompany/mysystem/development") == 0);
        CHECK(amqp_string_is_null(&open->hostname));

        CHECK_EQUAL(512U, open->max_frame_size);
        CHECK_EQUAL(255U, open->channel_max);
        CHECK_EQUAL(15000U, open->idle_time_out);

        CHECK(amqp_multiple_symbol_is_null(&open->outgoing_locales));
        CHECK(amqp_multiple_symbol_is_null(&open->incoming_locales));
        CHECK(amqp_multiple_symbol_is_null(&open->offered_capabilities));
        CHECK(amqp_multiple_symbol_is_null(&open->desired_capabilities));

        CHECK(amqp_map_is_null_or_empty(&open->properties));
    }

    TEST_FIXTURE(FrameFixture, client_begin_frame)
    {
        test_data::client_begin_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_begin_t *begin = &frame->frames.amqp.begin;

//        amqp_type_print(context, frame->type);

        CHECK_EQUAL(0U, begin->remote_channel);   // TODO -  was null, not 0 in frame
        CHECK_EQUAL(0U, begin->next_outgoing_id.value);
        CHECK_EQUAL(2147483647U, begin->incoming_window);
        CHECK_EQUAL(2147483647U, begin->outgoing_window);
        CHECK_EQUAL(15U, begin->handle_max.value);

        CHECK(amqp_multiple_symbol_is_null(&begin->offered_capabilities));
        CHECK(amqp_multiple_symbol_is_null(&begin->desired_capabilities));

        CHECK(amqp_map_is_null_or_empty(&begin->properties));
    }

    TEST_FIXTURE(FrameFixture, broker_begin_frame)
    {
        test_data::broker_begin_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_begin_t *begin = &frame->frames.amqp.begin;

//        amqp_type_print(context, frame->type);

        CHECK_EQUAL(0U, begin->remote_channel);
        CHECK_EQUAL(0U, begin->next_outgoing_id.value);
        CHECK_EQUAL(2147483647U, begin->incoming_window);
        CHECK_EQUAL(2147483647U, begin->outgoing_window);
        CHECK_EQUAL(15U, begin->handle_max.value);

        CHECK(amqp_multiple_symbol_is_null(&begin->offered_capabilities));
        CHECK(amqp_multiple_symbol_is_null(&begin->desired_capabilities));

        CHECK(amqp_map_is_null_or_empty(&begin->properties));
    }

    TEST_FIXTURE(FrameFixture, close_frame)
    {
        test_data::close_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_close_t *close = &frame->frames.amqp.close;

//        amqp_type_print(context, frame->type);

        CHECK(!close->error.is_null);
        CHECK(amqp_symbol_compare_with_cstr(&close->error.condition, "amqp:connection:forced") == 0);
        CHECK(amqp_string_compare_with_cstr(&close->error.description, "Our idle time-out occurred (we have not received any frames in idle-time-out x 2)") == 0);
        CHECK(amqp_map_is_null_or_empty(&close->error.info));
    }

    TEST_FIXTURE(FrameFixture, close_confirm_frame)
    {
        test_data::close_confirm_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_close_t *close = &frame->frames.amqp.close;

//        amqp_type_print(context, frame->type);

        CHECK(close->error.is_null);
    }

    TEST_FIXTURE(FrameFixture, client_attach_frame)
    {
        test_data::client_attach_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_attach_t *attach = &frame->frames.amqp.attach;

//        amqp_type_print(context, frame->type);

        CHECK(amqp_string_compare_with_cstr(&attach->name, "sender-link-name") == 0);
        CHECK_EQUAL(0U, attach->handle.value);
        CHECK(!attach->role);
        CHECK(attach->snd_settle_mode == amqp_sender_settle_mode_settled);
        CHECK(attach->rcv_settle_mode == amqp_receiver_settle_mode_first); // TODO - was null in frame
        CHECK(!amqp_wildcard_is_null(&attach->source));
//        amqp_type_print(context, attach->source.type);

        CHECK(!amqp_wildcard_is_null(&attach->target));
//        amqp_type_print(context, attach->target.type);
        CHECK(amqp_map_is_null_or_empty(&attach->unsettled));
        CHECK(attach->incomplete_unsettled == false);
        CHECK_EQUAL(0U, attach->initial_delivery_count.value);
        CHECK_EQUAL(0U, attach->max_message_size); // TODO - not provided
        CHECK(amqp_multiple_symbol_is_null(&attach->offered_capabilities));
        CHECK(amqp_multiple_symbol_is_null(&attach->desired_capabilities));
        CHECK(amqp_map_is_null_or_empty(&attach->properties));
    }

    TEST_FIXTURE(FrameFixture, broker_attach_frame)
    {
        test_data::broker_attach_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_attach_t *attach = &frame->frames.amqp.attach;

//        amqp_type_print(context, frame->type);

        CHECK(amqp_string_compare_with_cstr(&attach->name, "sender-link-name") == 0);
        CHECK_EQUAL(0U, attach->handle.value);
        CHECK(attach->role);
        CHECK(attach->snd_settle_mode == amqp_sender_settle_mode_unsettled);
        CHECK(attach->rcv_settle_mode == amqp_receiver_settle_mode_first); // TODO - was null in frame
        CHECK(!amqp_wildcard_is_null(&attach->source));
//        amqp_type_print(context, attach->source.type);

        CHECK(!amqp_wildcard_is_null(&attach->target));
//        amqp_type_print(context, attach->target.type);

        CHECK(amqp_map_is_null_or_empty(&attach->unsettled));
        CHECK(attach->incomplete_unsettled == false);           // TODO - not provided
        CHECK_EQUAL(0U, attach->initial_delivery_count.value);
        CHECK_EQUAL(0U, attach->max_message_size); // TODO - not provided
        CHECK(amqp_multiple_symbol_is_null(&attach->offered_capabilities));
        CHECK(amqp_multiple_symbol_is_null(&attach->desired_capabilities));
        CHECK(amqp_map_is_null_or_empty(&attach->properties));
    }

    TEST_FIXTURE(FrameFixture, flow_frame)
    {
        test_data::flow_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_flow_t *flow = &frame->frames.amqp.flow;

//        amqp_type_print(context, frame->type);

        CHECK_EQUAL(0U, flow->next_incoming_id.value);
        CHECK(flow->next_incoming_id.provided);

        CHECK_EQUAL(2147483647U, flow->incoming_window);

        CHECK_EQUAL(0U, flow->next_outgoing_id.value);
        CHECK(flow->next_outgoing_id.provided);

        CHECK_EQUAL(2147483647U, flow->outgoing_window);

        CHECK_EQUAL(0U, flow->handle.value);
        CHECK(flow->handle.provided);

        CHECK_EQUAL(0U, flow->delivery_count.value);
        CHECK(flow->delivery_count.provided);

        CHECK_EQUAL(0U, flow->link_credit);
        CHECK_EQUAL(0U, flow->available);
        CHECK(!flow->drain);
        CHECK(!flow->echo);

        CHECK(amqp_map_is_null_or_empty(&flow->properties));
    }
    TEST_FIXTURE(FrameFixture, broker_flow_frame)
    {
        test_data::broker_flow_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_flow_t *flow = &frame->frames.amqp.flow;

//        amqp_type_print(context, frame->type);

        CHECK_EQUAL(0U, flow->next_incoming_id.value);
        CHECK(flow->next_incoming_id.provided);

        CHECK_EQUAL(2147483647U, flow->incoming_window);

        CHECK_EQUAL(0U, flow->next_outgoing_id.value);
        CHECK(flow->next_outgoing_id.provided);

        CHECK_EQUAL(2147483647U, flow->outgoing_window);

        CHECK_EQUAL(0U, flow->handle.value);
        CHECK(flow->handle.provided);

        CHECK_EQUAL(0U, flow->delivery_count.value);
        CHECK(flow->delivery_count.provided);

        CHECK_EQUAL(4096U, flow->link_credit);
        CHECK_EQUAL(0U, flow->available);
        CHECK(!flow->drain);
        CHECK(!flow->echo);

        CHECK(amqp_map_is_null_or_empty(&flow->properties));
    }

    TEST_FIXTURE(FrameFixture, ya_flow_frame)
    {
        test_data::ya_flow_frame.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_flow_t *flow = &frame->frames.amqp.flow;

//        amqp_type_print(context, frame->type);

        CHECK_EQUAL(0U, flow->next_incoming_id.value);
        CHECK(flow->next_incoming_id.provided);

        CHECK_EQUAL(2147483647U, flow->incoming_window);

        CHECK_EQUAL(0U, flow->next_outgoing_id.value);
        CHECK(flow->next_outgoing_id.provided);

        CHECK_EQUAL(2147483647U, flow->outgoing_window);

        CHECK_EQUAL(0U, flow->handle.value);
        CHECK(flow->handle.provided);

        CHECK_EQUAL(0U, flow->delivery_count.value);
        CHECK(flow->delivery_count.provided);

        CHECK_EQUAL(4096U, flow->link_credit);
        CHECK_EQUAL(0U, flow->available);
        CHECK(!flow->drain);
        CHECK(!flow->echo);

        CHECK(amqp_map_is_null_or_empty(&flow->properties));
    }

    TEST_FIXTURE(FrameFixture, transfer_frame_id_0)
    {
        test_data::transfer_frame_id_0.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_transfer_t *transfer = &frame->frames.amqp.transfer;

        CHECK_EQUAL(0U, transfer->handle.value);
        CHECK(transfer->handle.provided);

        CHECK_EQUAL(0U, transfer->delivery_id.value);
        CHECK(transfer->delivery_id.provided);
        CHECK_EQUAL(0U, transfer->message_format.value);
        CHECK(transfer->message_format.provided);
        CHECK(transfer->settled);
        CHECK(!transfer->more);
        CHECK(amqp_wildcard_is_null(&transfer->state));
        CHECK(!transfer->resume);
        CHECK(!transfer->aborted);
        CHECK(!transfer->batchable);
    }
    
    TEST_FIXTURE(FrameFixture, transfer_frame_id_256)
    {
        test_data::transfer_frame_id_256.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_transfer_t *transfer = &frame->frames.amqp.transfer;

        CHECK_EQUAL(0U, transfer->handle.value);
        CHECK(transfer->handle.provided);
        CHECK_EQUAL(256U, transfer->delivery_id.value);
        CHECK(transfer->delivery_id.provided);
        CHECK_EQUAL(0U, transfer->message_format.value);
        CHECK(transfer->message_format.provided);
        CHECK(transfer->settled);
        CHECK(!transfer->more);
        CHECK(amqp_wildcard_is_null(&transfer->state));
        CHECK(!transfer->resume);
        CHECK(!transfer->aborted);
        CHECK(!transfer->batchable);
    }

    TEST_FIXTURE(FrameFixture, transfer_frame_id_677)
    {
        test_data::transfer_frame_id_677.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);

        ASSERT(frame != 0);
        CHECK(check_amqp_header());

        amqp_transport_transfer_t *transfer = &frame->frames.amqp.transfer;

        CHECK_EQUAL(0U, transfer->handle.value);
        CHECK(transfer->handle.provided);
        CHECK_EQUAL(677U, transfer->delivery_id.value);
        CHECK(transfer->delivery_id.provided);
        CHECK_EQUAL(0U, transfer->message_format.value);
        CHECK(transfer->message_format.provided);
        CHECK(transfer->settled);
        CHECK(!transfer->more);
        CHECK(amqp_wildcard_is_null(&transfer->state));
        CHECK(!transfer->resume);
        CHECK(!transfer->aborted);
        CHECK(!transfer->batchable);
    }
}
