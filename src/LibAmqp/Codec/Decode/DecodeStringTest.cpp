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


SUITE(StringDecoder)
{
    class DecodeFixture : public SuiteCodec::CodecFixture
    {
    public:
        DecodeFixture() : result(0) {}
        ~DecodeFixture()
        {
            AMQP_FREE(result);
        }

    public:
        char *result;
    };
        
    TEST_FIXTURE(DecodeFixture, UTF8String)
    {
        load_decode_buffer(test_data::hello_world);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        CHECK_EQUAL(0xa1, type->format_code);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 0x0b, type->position.size);
        CHECK_EQUAL("Hello World", (result = amqp_convert_bytes_to_cstr(context, type)));
    }

    TEST_FIXTURE(DecodeFixture, LongUtf8String)
    {
        load_decode_buffer(test_data::hello_big_world);
        type = amqp_decode(context);

        ASSERT_VALID(type);
        CHECK_EQUAL(0xb1, type->format_code);
        CHECK_EQUAL((size_t) 0x05, type->position.index);
        CHECK_EQUAL((size_t) 0x0f, type->position.size);
        CHECK_EQUAL("Hello Big World", (result = amqp_convert_bytes_to_cstr(context, type)));
    }
}
