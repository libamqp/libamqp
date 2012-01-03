/*
   Copyright 2011-2012 StormMQ Limited

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
#include "debug_helper.h"

SUITE(Frame)
{
    TEST_FIXTURE(FrameFixture, fixture_should_balance_allocations)
    {
    }

    TEST_FIXTURE(FrameFixture, decode_minimal_frame)
    {
        test_data::minimal_frame_header.transfer_to(decode_buffer);
        frame = amqp_decode_amqp_frame(context, decode_buffer);
        ASSERT(frame != 0);

        CHECK_EQUAL(8U, frame->data_offset);
        CHECK_EQUAL(AMQP_FRAME_TYPE, frame->frame_type);
        CHECK_EQUAL(1U, frame->type_specific.word);
        CHECK_EQUAL(amqp_empty_frame, frame->descriptor.group);

        CHECK_EQUAL(8U, amqp_buffer_index(decode_buffer));
        CHECK_EQUAL(0, amqp_buffer_available(decode_buffer));
    }

    TEST_FIXTURE(FrameFixture, sasl_mechanisms_frame_with_invalid_descriptor_id)
    {
        test_data::sasl_mechanisms_frame_with_invalid_descriptor_id.transfer_to(decode_buffer);
        context->debug.level = 0;
        frame = amqp_decode_sasl_frame(context, decode_buffer);
        ASSERT(frame == 0);
    }

    TEST_FIXTURE(FrameFixture, decode_sasl_mechanisms_frame)
    {
        test_data::sasl_mechanisms_frame.transfer_to(decode_buffer);
        frame = amqp_decode_sasl_frame(context, decode_buffer);
        ASSERT(frame != 0);

        CHECK(check_sasl_header());

        amqp_multiple_symbol_t *multiple = &frame->frames.sasl.mechanisms.sasl_server_mechanisms;
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 0), "PLAIN") == 0);
    }

    TEST_FIXTURE(FrameFixture, sasl_mechanisms_frame_long)
    {
        test_data::sasl_mechanisms_frame_long.transfer_to(decode_buffer);
        frame = amqp_decode_sasl_frame(context, decode_buffer);
        ASSERT(frame != 0);

        CHECK(check_sasl_header());

        amqp_multiple_symbol_t *multiple = &frame->frames.sasl.mechanisms.sasl_server_mechanisms;
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 0), "PLAIN") == 0);
    }

    TEST_FIXTURE(FrameFixture, sasl_mechanisms_frame_symbol)
    {
        test_data::sasl_mechanisms_frame_symbol.transfer_to(decode_buffer);
        frame = amqp_decode_sasl_frame(context, decode_buffer);
        ASSERT(frame != 0);

        CHECK(check_sasl_header());

        amqp_multiple_symbol_t *multiple = &frame->frames.sasl.mechanisms.sasl_server_mechanisms;
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 0), "ANONYMOUS") == 0);
    }

    TEST_FIXTURE(FrameFixture, sasl_init_frame)
    {
        test_data::sasl_init_frame.transfer_to(decode_buffer);
        frame = amqp_decode_sasl_frame(context, decode_buffer);

        ASSERT(frame != 0);

        CHECK(check_sasl_header());

        CHECK(amqp_symbol_compare_with_cstr(&frame->frames.sasl.init.mechanism, "PLAIN") == 0);

        CHECK_EQUAL(0, amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 0));
        CHECK_EQUAL('a', amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 1));
        CHECK_EQUAL(0, amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 8));
        CHECK_EQUAL('s', amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 9));

        CHECK(amqp_string_compare_with_cstr(&frame->frames.sasl.init.hostname, "localhost.localdomain") == 0);
    }

    TEST_FIXTURE(FrameFixture, sasl_init_frame_captured)
    {
        test_data::sasl_init_frame_captured.transfer_to(decode_buffer);
        frame = amqp_decode_sasl_frame(context, decode_buffer);
        ASSERT(frame != 0);

        CHECK(check_sasl_header());

        CHECK(amqp_symbol_compare_with_cstr(&frame->frames.sasl.init.mechanism, "PLAIN") == 0);

        CHECK_EQUAL(262U, amqp_binary_size(&frame->frames.sasl.init.initial_response));
        CHECK_EQUAL(0, amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 0));
        CHECK_EQUAL('r', amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 1));
        CHECK_EQUAL('a', amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 2));
        CHECK_EQUAL('p', amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 3));
        CHECK_EQUAL('h', amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 4));
        CHECK_EQUAL(0, amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 5));

        CHECK(amqp_string_compare_with_cstr(&frame->frames.sasl.init.hostname, "development.mysystem.mycompany.stormmq.com") == 0);
    }

    TEST_FIXTURE(FrameFixture, sasl_outcome_frame)
    {
        test_data::sasl_outcome_frame.transfer_to(decode_buffer);
        frame = amqp_decode_sasl_frame(context, decode_buffer);
        ASSERT(frame != 0);

        CHECK(check_sasl_header());
        CHECK_EQUAL(0U, frame->frames.sasl.outcome.code);
        CHECK(amqp_binary_is_null(&frame->frames.sasl.outcome.additional_data));
    }

    TEST_FIXTURE(FrameFixture, sasl_outcome_frame_auth_error)
    {
        test_data::sasl_outcome_frame_auth_error.transfer_to(decode_buffer);
        frame = amqp_decode_sasl_frame(context, decode_buffer);
        ASSERT(frame != 0);

        CHECK(check_sasl_header());
        CHECK_EQUAL((unsigned) amqp_sasl_code_auth_error, frame->frames.sasl.outcome.code);
        CHECK(!amqp_binary_is_null(&frame->frames.sasl.outcome.additional_data));
        CHECK_EQUAL(4U, amqp_binary_size(&frame->frames.sasl.outcome.additional_data));
    }

}
