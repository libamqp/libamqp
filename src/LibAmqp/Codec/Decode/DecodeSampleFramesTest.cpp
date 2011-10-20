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
        load(test_data::sasl_init);
        CHECK(amqp_type_is_ulong(descriptor));
        CHECK_EQUAL(65U, amqp_type_to_ulong(descriptor));
        amqp_type_print(context, described, decode_buffer);
        not_implemented(todo);
    }
}