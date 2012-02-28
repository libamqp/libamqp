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
#include "Context/ErrorHandling.h"

#include "Context/TestSupport/ContextHolder.h"
#include "Context/TestSupport/DecodeBufferHolder.h"
#include "Context/TestSupport/TypeHolder.h"

#include "Codec/Decode/Decode.h"

#include "Codec/Type/Type.h"
#include "Codec/Type/TypePrint.h"
#include "TestData/TestFrames.h"
#include "Codec/Type/TypeExtension.h"
#include "Codec/Type/FieldTests.h"

#include "debug_helper.h"

SUITE(FrameTypeDecode)
{
    // These tests ensure that the AMQP encoded part of a frame can be decoded. If these
    // fail the problem is most likely the test data, not the frame decoder

    class FrameTypeDecodeTestFixture : 
            public virtual TestSupport::ContextHolder,
            public virtual TestSupport::DecodeBufferHolder,
            public virtual TestSupport::TypeHolder
    {
    public:
        FrameTypeDecodeTestFixture() : result(0) {}
        ~FrameTypeDecodeTestFixture()
        {
            AMQP_FREE(context, result);
        }
        void load(test_data::TestData &data);

    public:
        char *result;
        amqp_type_t *descriptor;
        amqp_type_t *described;
    };

    TEST_FIXTURE(FrameTypeDecodeTestFixture, multiple_symbol_null)
    {
        test_data::multiple_symbol_null.transfer_to(decode_buffer);
        type = amqp_decode(context, decode_buffer);

        CHECK(amqp_type_is_null(type));
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, multiple_symbol_one_value)
    {
        test_data::multiple_symbol_one_value.transfer_to(decode_buffer);
        type = amqp_decode(context, decode_buffer);

        CHECK(amqp_type_is_symbol(type));
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, multiple_symbol_many_values)
    {
        test_data::multiple_symbol_many_values.transfer_to(decode_buffer);
        type = amqp_decode(context, decode_buffer);

        CHECK(amqp_type_is_array(type));
        CHECK_EQUAL(3U, type->value.array.count);
        CHECK(amqp_type_is_symbol(type->value.array.elements[0]));
    }

    void FrameTypeDecodeTestFixture::load(test_data::TestData &data)
    {
        load_decode_buffer(data);
        amqp_buffer_advance_read_index(decode_buffer, 8);
        type = amqp_decode(context, decode_buffer);
        CHECK(amqp_type_is_valid(type));
        CHECK(amqp_type_is_composite(type));
        descriptor = amqp_type_get_descriptor(type);
        CHECK(amqp_type_is_ulong(descriptor) || amqp_type_is_symbol(descriptor));

        described = amqp_type_get_described(type);
        CHECK(amqp_type_is_list(described));
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, sasl_mechanisms_frame_with_invalid_descriptor_id)
    {
        load(test_data::sasl_mechanisms_frame_with_invalid_descriptor_id);
        CHECK_EQUAL(63U, amqp_type_to_ulong(descriptor));
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, test_decode_of_sasl_mechanisms_frame)
    {
        load(test_data::sasl_mechanisms_frame);
        CHECK_EQUAL(64U, amqp_type_to_ulong(descriptor));
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, test_decode_of_sasl_mechanisms_frame_using_large_long)
    {
        load(test_data::sasl_mechanisms_frame_long);
        CHECK_EQUAL(64U, amqp_type_to_ulong(descriptor));
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, test_decode_of_sasl_mechanisms_frame_using_symbol)
    {
        load(test_data::sasl_mechanisms_frame_symbol);
        CHECK(amqp_type_is_symbol(descriptor));
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, test_decode_of_sasl_init)
    {
        load(test_data::sasl_init_frame);
        CHECK_EQUAL(65U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(3U, described->value.list.count);
        CHECK(amqp_type_is_symbol(described->value.list.elements[0]));
        CHECK(amqp_type_is_binary(described->value.list.elements[1]));
        CHECK(amqp_type_is_string(described->value.list.elements[2]));
    }
    TEST_FIXTURE(FrameTypeDecodeTestFixture, test_decode_of_sasl_init_frame_captured)
    {
        load(test_data::sasl_init_frame_captured);
        CHECK_EQUAL(65U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(3U, described->value.list.count);
        CHECK(amqp_type_is_symbol(described->value.list.elements[0]));
        CHECK(amqp_type_is_binary(described->value.list.elements[1]));
        CHECK(amqp_type_is_string(described->value.list.elements[2]));
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, sasl_outcome_frame_auth_error)
    {
        load(test_data::sasl_outcome_frame_auth_error);

        CHECK_EQUAL(68U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(2U, described->value.list.count);
        CHECK(amqp_type_is_ubyte(described->value.list.elements[0]));
        CHECK(amqp_type_is_binary(described->value.list.elements[1]));
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, client_open_frame)
    {
        load(test_data::client_open_frame);

        CHECK_EQUAL(16U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(10U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_string(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_string(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_ushort(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, broker_open_frame)
    {
        load(test_data::broker_open_frame);

        CHECK_EQUAL(16U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(10U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_string(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_ushort(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, client_begin_frame)
    {
        load(test_data::client_begin_frame);

        CHECK_EQUAL(17U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(8U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, broker_begin_frame)
    {
        load(test_data::broker_begin_frame);

        CHECK_EQUAL(17U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(8U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_ushort(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, close_frame)
    {
        load(test_data::close_frame);

        CHECK_EQUAL(0x18U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(1U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_composite(described->value.list.elements[field])); field++;
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, close_confirm_frame)
    {
        load(test_data::close_confirm_frame);

        CHECK_EQUAL(0x18U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(0U, described->value.list.count);

    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, client_attach_frame)
    {
        load(test_data::client_attach_frame);

        CHECK_EQUAL(0x12U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(14U, described->value.list.count);


        int field = 0;
        CHECK(amqp_type_is_string(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_role(described->value.list.elements[field])); field++;

        CHECK(amqp_field_is_sender_settle_mode(described->value.list.elements[field])); field++;

        amqp_field_is_receiver_settle_mode(described->value.list.elements[field]); field++;

        CHECK(amqp_field_is_wildcard(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_wildcard(described->value.list.elements[field])); field++;

        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_boolean(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_sequence_no(described->value.list.elements[field])); field++;

        CHECK(amqp_type_is_null(described->value.list.elements[field]) || amqp_type_is_ulong(described->value.list.elements[field])); field++;

        CHECK(amqp_field_is_multiple_symbol(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_multiple_symbol(described->value.list.elements[field])); field++;

        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;

        CHECK_EQUAL(14, field);
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, broker_attach_frame)
    {
        load(test_data::broker_attach_frame);

        CHECK_EQUAL(0x12U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(14U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_string(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_role(described->value.list.elements[field])); field++;

        CHECK(amqp_field_is_sender_settle_mode(described->value.list.elements[field])); field++;

        amqp_field_is_receiver_settle_mode(described->value.list.elements[field]); field++;

        CHECK(amqp_field_is_wildcard(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_wildcard(described->value.list.elements[field])); field++;

        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_boolean(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_sequence_no(described->value.list.elements[field])); field++;

        CHECK(amqp_type_is_null(described->value.list.elements[field]) || amqp_type_is_ulong(described->value.list.elements[field])); field++;

        CHECK(amqp_field_is_multiple_symbol(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_multiple_symbol(described->value.list.elements[field])); field++;

        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;
        CHECK_EQUAL(14, field);
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, flow_frame)
    {
        load(test_data::flow_frame);
        CHECK_EQUAL(0x13U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(11U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;

        CHECK_EQUAL(11, field);
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, broker_flow_frame)
    {
        load(test_data::broker_flow_frame);

        CHECK_EQUAL(0x13U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(11U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;
        CHECK_EQUAL(11, field);
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, ya_flow_frame)
    {
        load(test_data::ya_flow_frame);

        CHECK_EQUAL(0x13U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(11U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_null(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_map(described->value.list.elements[field])); field++;
        CHECK_EQUAL(11, field);
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, transfer_frame_id_0)
    {
        load(test_data::transfer_frame_id_0);

        CHECK_EQUAL(0x14U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(6U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_delivery_number(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_delivery_tag(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_message_format(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_boolean(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_boolean(described->value.list.elements[field])); field++;

        CHECK_EQUAL(6, field);

        amqp_deallocate_type(context, type);
        type = amqp_decode(context, decode_buffer);
        ASSERT(type);
//        amqp_type_print(context,  described);
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, transfer_frame_id_256)
    {
        load(test_data::transfer_frame_id_256);

        CHECK_EQUAL(0x14U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(6U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_delivery_number(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_delivery_tag(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_message_format(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_boolean(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_boolean(described->value.list.elements[field])); field++;

        CHECK_EQUAL(6, field);

        amqp_deallocate_type(context, type);
        type = amqp_decode(context, decode_buffer);
        ASSERT(type);
//        amqp_type_print(context,  described);
    }

    TEST_FIXTURE(FrameTypeDecodeTestFixture, transfer_frame_id_677)
    {
        load(test_data::transfer_frame_id_677);

        CHECK_EQUAL(0x14U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(6U, described->value.list.count);

        int field = 0;
        CHECK(amqp_type_is_uint(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_delivery_number(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_delivery_tag(described->value.list.elements[field])); field++;
        CHECK(amqp_field_is_message_format(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_boolean(described->value.list.elements[field])); field++;
        CHECK(amqp_type_is_boolean(described->value.list.elements[field])); field++;

        CHECK_EQUAL(6, field);

        amqp_deallocate_type(context, type);
        type = amqp_decode(context, decode_buffer);
        ASSERT(type);
//        amqp_type_print(context,  described);
    }
}