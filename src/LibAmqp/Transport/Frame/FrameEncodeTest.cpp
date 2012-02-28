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
#include "Transport/Frame/FrameEncode.h"
#include "Transport/Frame/FrameTestFixture.h"

#include "Codec/Decode/Decode.h"
#include "debug_helper.h"

SUITE(Frame)
{
    TEST_FIXTURE(FrameTestFixture, sasl_mechanisms_frame_encode)
    {
        assert(connection != 0);
        amqp_buffer_t *buffer = amqp_encode_sasl_mechanisms_frame(connection);

        frame = amqp_decode_sasl_frame(context, buffer);
        amqp_deallocate_buffer(context, buffer);

        ASSERT(frame != 0);

        CHECK_EQUAL(8U, frame->data_offset);
        CHECK_EQUAL(AMQP_SASL_FRAME_TYPE, frame->frame_type);
        CHECK_EQUAL(0U, frame->type_specific.word);

        amqp_multiple_symbol_t *multiple = &frame->frames.sasl.mechanisms.sasl_server_mechanisms;
        ASSERT(amqp_multiple_symbol_size(multiple) > 0);

        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 0), "PLAIN") == 0);
    }

    static
    amqp_sasl_plugin_t *lookup_sasl_plugin(amqp_context_t *context, const char *mechanism_name)
    {
        amqp_symbol_t *mechanism = amqp_symbol_create_from_cstr(context, mechanism_name);
        amqp_sasl_plugin_t *result = amqp_context_lookup_sasl_plugin(context, mechanism);
        amqp_symbol_cleanup(context, mechanism);
        return result;
    }

    TEST_FIXTURE(FrameTestFixture, sasl_init_frame_encode)
    {
        sasl_plugin = lookup_sasl_plugin(context, "PLAIN");
        CHECK(sasl_plugin);

        amqp_buffer_t *buffer = amqp_encode_sasl_init_frame(connection, sasl_plugin);

        frame = amqp_decode_sasl_frame(context, buffer);
        amqp_deallocate_buffer(context, buffer);
        
        ASSERT(frame != 0);

        CHECK_EQUAL(8U, frame->data_offset);
        CHECK_EQUAL(AMQP_SASL_FRAME_TYPE, frame->frame_type);
        CHECK_EQUAL(0U, frame->type_specific.word);

        CHECK_EQUAL((uint32_t) AMQP_DESCRIPTOR_DOMAIN, frame->descriptor.domain);
        CHECK_EQUAL((uint32_t) amqp_sasl_init_list_descriptor, frame->descriptor.id);

        CHECK_EQUAL(0, amqp_symbol_compare_with_cstr(&frame->frames.sasl.init.mechanism, "PLAIN"));
        CHECK_EQUAL('j', amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 1));
        CHECK_EQUAL('s', amqp_binary_byte_get_at(&frame->frames.sasl.init.initial_response, 5));
        CHECK_EQUAL(0, amqp_string_compare_with_cstr(&frame->frames.sasl.init.hostname, "unknown"));
    }
}
