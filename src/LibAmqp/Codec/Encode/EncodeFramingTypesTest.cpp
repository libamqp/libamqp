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
#include "Codec/Encode/EncodeFramingTypes.h"

#include <stdio.h>

SUITE(FramingTypesEncoding)
{
    class EncodeFixture : public SuiteCodec::CodecFixture
    {
    public:
        EncodeFixture() : result(0) {}
        ~EncodeFixture()
        {
            AMQP_FREE(result);
            deallocate_type(result);
        }

    public:
        amqp_type_t *result;
    };

    TEST_FIXTURE(EncodeFixture, multiple_true_symbol_null)
    {
        type = amqp_encode_multiple_true(context, amqp_type_d(symbol));
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::multiple_true_symbol_null);
    }

    TEST_FIXTURE(EncodeFixture, multiple_true_symbol_one_value)
    {
        type =  amqp_encode_multiple_true(context, amqp_type_d(symbol), "Foo");
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::multiple_true_symbol_one_value);
    }

    TEST_FIXTURE(EncodeFixture, multiple_true_symbol_many_values)
    {
        type =  amqp_encode_multiple_true(context, amqp_type_d(symbol), "Foo", "Fum");
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::multiple_true_symbol_many_values);
    }
}