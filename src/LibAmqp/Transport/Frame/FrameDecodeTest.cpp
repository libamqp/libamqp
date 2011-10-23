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
#include "Context/ErrorHandling.h"

#include "Codec/CodecTestSupport.h"
#include "Codec/Decode/Decode.h"

#include "Codec/Type/Type.h"
#include "Codec/Type/TypePrint.h"
#include "TestData/TestFrames.h"
#include "Codec/Type/TypeExtension.h"

SUITE(FrameTypeDecode)
{
    // These tests ensure that the AMQP encoded part of a frame can be decoded.

    class DecodeFixture : public SuiteCodec::CodecFixture
    {
    public:
        DecodeFixture() : result(0) {}
        ~DecodeFixture()
        {
            AMQP_FREE(context, result);
        }
        void load(test_data::TestData &data);

    public:
        char *result;
        amqp_type_t *descriptor;
        amqp_type_t *described;
    };


    TEST_FIXTURE(DecodeFixture, multiple_symbol_null)
    {
        test_data::multiple_symbol_null.transfer_to(decode_buffer);
        type = amqp_decode(context, decode_buffer);

        CHECK(amqp_type_is_null(type));
    }

    TEST_FIXTURE(DecodeFixture, multiple_symbol_one_value)
    {
        test_data::multiple_symbol_one_value.transfer_to(decode_buffer);
        type = amqp_decode(context, decode_buffer);

        CHECK(amqp_type_is_symbol(type));
    }

    TEST_FIXTURE(DecodeFixture, multiple_symbol_many_values)
    {
        test_data::multiple_symbol_many_values.transfer_to(decode_buffer);
        type = amqp_decode(context, decode_buffer);

        CHECK(amqp_type_is_array(type));
        CHECK_EQUAL(3U, type->value.array.count);
        CHECK(amqp_type_is_symbol(type->value.array.elements[0]));
    }

    TEST_FIXTURE(DecodeFixture, multiple_symbol_empty_array)
    {
        // TODO - FIX
        /*
        test_data::multiple_symbol_empty_array.transfer_to(buffer);
        type = amqp_decode(context, buffer);

        CHECK(amqp_type_is_array(type));
        CHECK_EQUAL(0U, type->value.array.count);
        */
//        CHECK(amqp_type_is_symbol(type->value.array.elements[0]));
    }

    void DecodeFixture::load(test_data::TestData &data)
    {
        load_decode_buffer(data);
        amqp_buffer_advance_read_index(decode_buffer, 8);
        type = amqp_decode(context, decode_buffer);
        CHECK(amqp_type_is_valid(type));
        CHECK(amqp_type_is_described(type));
        descriptor = amqp_type_get_descriptor(type);
        CHECK(amqp_type_is_ulong(descriptor) || amqp_type_is_symbol(descriptor));

        described = amqp_type_get_described(type);
        CHECK(amqp_type_is_list(described));
    }

    TEST_FIXTURE(DecodeFixture, sasl_mechanisms_frame_with_invalid_descriptor_id)
    {
        load(test_data::sasl_mechanisms_frame_with_invalid_descriptor_id);
        CHECK_EQUAL(63U, amqp_type_to_ulong(descriptor));
    }

    TEST_FIXTURE(DecodeFixture, test_decode_of_sasl_mechanisms_frame)
    {
        load(test_data::sasl_mechanisms_frame);
        CHECK_EQUAL(64U, amqp_type_to_ulong(descriptor));
    }

    TEST_FIXTURE(DecodeFixture, test_decode_of_sasl_mechanisms_frame_using_large_long)
    {
        load(test_data::sasl_mechanisms_frame_long);
        CHECK_EQUAL(64U, amqp_type_to_ulong(descriptor));
    }

    TEST_FIXTURE(DecodeFixture, test_decode_of_sasl_mechanisms_frame_using_symbol)
    {
        load(test_data::sasl_mechanisms_frame_symbol);
        CHECK(amqp_type_is_symbol(descriptor));
    }

    TEST_FIXTURE(DecodeFixture, test_decode_of_sasl_init)
    {
        load(test_data::sasl_init_frame);
        CHECK(amqp_type_is_ulong(descriptor));
        CHECK_EQUAL(65U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(3U, described->value.list.count);
        CHECK(amqp_type_is_symbol(described->value.list.elements[0]));
        CHECK(amqp_type_is_binary(described->value.list.elements[1]));
        CHECK(amqp_type_is_string(described->value.list.elements[2]));
    }
    TEST_FIXTURE(DecodeFixture, test_decode_of_sasl_init_frame_captured)
    {
        load(test_data::sasl_init_frame_captured);
        CHECK(amqp_type_is_ulong(descriptor));
        CHECK_EQUAL(65U, amqp_type_to_ulong(descriptor));
        CHECK_EQUAL(3U, described->value.list.count);
        CHECK(amqp_type_is_symbol(described->value.list.elements[0]));
        CHECK(amqp_type_is_binary(described->value.list.elements[1]));
        CHECK(amqp_type_is_string(described->value.list.elements[2]));
    }
}