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

#include "Codec/Decode/DecodeTestFixture.h"

#include "Codec/Decode/Decode.h"
#include "Codec/Type/TypeExtension.h"

#include "Codec/Type/Type.h"

SUITE(CodecDecode)
{
    TEST_FIXTURE(DecodeTestFixture, decode_binary)
    {
        load_decode_buffer(test_data::bin_8);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK_EQUAL(0xa0, type->constructor.format_code);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 12, type->position.size);
        CHECK(amqp_type_is_variable(type));
        CHECK(amqp_type_is_binary(type));
        CHECK_EQUAL("binary array", (result = amqp_convert_bytes_to_cstr(context, type)));
    }

    TEST_FIXTURE(DecodeTestFixture, decode_large_binary)
    {
        load_decode_buffer(test_data::bin_32);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK_EQUAL(0xb0, type->constructor.format_code);
        CHECK_EQUAL((size_t) 0x05, type->position.index);
        CHECK_EQUAL((size_t) 16, type->position.size);
        CHECK(amqp_type_is_variable(type));
        CHECK(amqp_type_is_binary(type));
        CHECK_EQUAL("big binary array", (result = amqp_convert_bytes_to_cstr(context, type)));
    }

    TEST_FIXTURE(DecodeTestFixture, UTF8String)
    {
        load_decode_buffer(test_data::hello_world);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK_EQUAL(0xa1, type->constructor.format_code);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 0x0b, type->position.size);
        CHECK(amqp_type_is_variable(type));
        CHECK(amqp_type_is_string(type));
        CHECK_EQUAL("Hello World", (result = amqp_convert_bytes_to_cstr(context, type)));
    }

    TEST_FIXTURE(DecodeTestFixture, LongUtf8String)
    {
        load_decode_buffer(test_data::hello_big_world);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK_EQUAL(0xb1, type->constructor.format_code);
        CHECK_EQUAL((size_t) 0x05, type->position.index);
        CHECK_EQUAL((size_t) 0x0f, type->position.size);
        CHECK(amqp_type_is_variable(type));
        CHECK(amqp_type_is_string(type));
        CHECK_EQUAL("Hello Big World", (result = amqp_convert_bytes_to_cstr(context, type)));
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeLargeSymbol)
    {
        load_decode_buffer(test_data::foo_bar_symbol_32);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK_EQUAL(0xb3, type->constructor.format_code);
        CHECK_EQUAL((size_t) 0x05, type->position.index);
        CHECK_EQUAL((size_t) 0x06, type->position.size);
        CHECK(amqp_type_is_variable(type));
        CHECK(amqp_type_is_symbol(type));
        CHECK_EQUAL("FooBar", (result = amqp_convert_bytes_to_cstr(context, type)));
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeSymbol)
    {
        load_decode_buffer(test_data::foo_symbol_8);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK_EQUAL(0xa3, type->constructor.format_code);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 0x03, type->position.size);
        CHECK(amqp_type_is_variable(type));
        CHECK(amqp_type_is_symbol(type));
        CHECK_EQUAL("Foo", (result = amqp_convert_bytes_to_cstr(context, type)));
    }

    TEST_FIXTURE(DecodeTestFixture, DecodeInvalidSymbol)
    {
        load_decode_buffer(test_data::bad_symbol);
        context->debug.level = 0;
        type = amqp_decode(context, decode_buffer);

        ASSERT_INVALID(type);
        CHECK_EQUAL(AMQP_ERROR_INVALID_SYMBOL_CHARACTER, type->invalid_cause);
    }
}
