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


SUITE(SymbolDecode)
{
    class DecodeFixture : public SuiteCodec::CodecFixture
    {
    public:
        DecodeFixture() : result(0) {}
        ~DecodeFixture()
        {
            AMQP_FREE(context, result);
        }

    public:
        char *result;
    };

    TEST_FIXTURE(DecodeFixture, DecodeLargeSymbol)
    {
        load_decode_buffer(test_data::foo_bar_symbol_32);
        type = amqp_decode(context, context->decode.buffer);

        ASSERT_VALID(type);
        CHECK_EQUAL(0xb3, type->format_code);
        CHECK_EQUAL((size_t) 0x05, type->position.index);
        CHECK_EQUAL((size_t) 0x06, type->position.size);
        CHECK_EQUAL("FooBar", (result = amqp_convert_bytes_to_cstr(context, type)));
    }

    TEST_FIXTURE(DecodeFixture, DecodeSymbol)
    {
        load_decode_buffer(test_data::foo_symbol_8);
        type = amqp_decode(context, context->decode.buffer);

        ASSERT_VALID(type);
        CHECK_EQUAL(0xa3, type->format_code);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 0x03, type->position.size);
        CHECK_EQUAL("Foo", (result = amqp_convert_bytes_to_cstr(context, type)));
    }

    TEST_FIXTURE(DecodeFixture, DecodeInvalidSymbol)
    {
        load_decode_buffer(test_data::bad_symbol);
        context->debug.level = 0;
        type = amqp_decode(context, context->decode.buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_INVALID_SYMBOL_CHARACTER, type->invalid_cause);
    }
}