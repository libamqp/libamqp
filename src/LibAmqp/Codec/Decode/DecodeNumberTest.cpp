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

SUITE(NumericDecode)
{
    class DecodeFixture : public SuiteCodec::CodecFixture
    {
    public:
        DecodeFixture() {}
        ~DecodeFixture()
        {
        }

    public:
    };

    TEST_FIXTURE(DecodeFixture, Float)
    {
        load_decode_buffer(test_data::float_4);

        type = amqp_decode(context);

        ASSERT_VALID(type);
        CHECK_CLOSE(123.456f, type->value.b4._float, 0.00005);
    }

    TEST_FIXTURE(DecodeFixture, Double)
    {
        load_decode_buffer(test_data::double_8);

        type = amqp_decode(context);

        ASSERT_VALID(type);
        CHECK_CLOSE(123.456, type->value.b8._double, 0.00005);
    }
}