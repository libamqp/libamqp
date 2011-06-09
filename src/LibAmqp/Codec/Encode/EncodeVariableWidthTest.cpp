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

#include <UnitTest++.h>
#include "ExtraChecks.h"
#include "Context/ErrorHandling.h"

#include "Codec/CodecTestSupport.h"
#include "Codec/Encode/Encode.h"

#include "Codec/Type/Type.h"

SUITE(VariableTypesEncoding)
{
    const unsigned char *binary = (unsigned char *) "binary array";
    const unsigned char *big_binary = (unsigned char *) "big binary array";

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
    
    TEST_FIXTURE(EncodeFixture, amqp_encode_string_str_utf8)
    {
        type = amqp_encode_string_utf8(context, "Hello World");

        CHECK_NOT_NULL(type);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::hello_world);

        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 0x0b, type->position.size);
    }

    TEST_FIXTURE(EncodeFixture, amqp_encode_string_utf8)
    {
        type = amqp_encode_string_utf8(context, "Hello World");

        CHECK_NOT_NULL(type);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::hello_world);

        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 0x0b, type->position.size);
    }

    TEST_FIXTURE(EncodeFixture, amqp_encode_string_str32_utf8)
    {
        CHECK_EQUAL((size_t) 257, strlen(test_data::lorem_ipsum));

        type = amqp_encode_string_utf8(context, test_data::lorem_ipsum);

        CHECK_NOT_NULL(type);

    //    amqp_buffer_dump_as_byte_array(context->encode.buffer, context->config.putc);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::encoded_lorem_ipsum);

        CHECK_EQUAL((size_t) 0x05, type->position.index);
        CHECK_EQUAL((size_t) 257, type->position.size);
    }

    TEST_FIXTURE(EncodeFixture, amqp_encode_binary)
    {
        type = amqp_encode_binary(context, binary, strlen((const char *) binary));

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 12, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::bin_8);
    }

    TEST_FIXTURE(EncodeFixture, amqp_encode_binary_bin8)
    {
        type = amqp_encode_binary_vbin8(context, binary, strlen((const char *) binary));

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 12, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::bin_8);
    }

    TEST_FIXTURE(EncodeFixture, amqp_encode_binary_bin32)
    {
        type = amqp_encode_binary_vbin32(context, big_binary, strlen((const char *) big_binary));

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x05, type->position.index);
        CHECK_EQUAL((size_t) 16, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::bin_32);
    }


    TEST_FIXTURE(EncodeFixture, amqp_encode_symbol)
    {
        type = amqp_encode_symbol(context, "Foo");

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 3, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::foo_symbol_8);
    }

    TEST_FIXTURE(EncodeFixture, amqp_encode_symbol_sym8)
    {
        type = amqp_encode_symbol_sym8(context, "Foo");

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 3, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::foo_symbol_8);
    }

    TEST_FIXTURE(EncodeFixture, amqp_encode_symbol_sym32)
    {
        type = amqp_encode_symbol_sym32(context, "FooBar");

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x05, type->position.index);
        CHECK_EQUAL((size_t) 6, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::foo_bar_symbol_32);
    }


    TEST_FIXTURE(EncodeFixture, amqp_encode_symboln)
    {
        type = amqp_encode_symboln(context, "Foo", 3);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 3, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::foo_symbol_8);
    }

    TEST_FIXTURE(EncodeFixture, amqp_encode_symbol_sym8n)
    {
        type = amqp_encode_symbol_sym8n(context, "Foo", 3);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x02, type->position.index);
        CHECK_EQUAL((size_t) 3, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::foo_symbol_8);
    }

    TEST_FIXTURE(EncodeFixture, amqp_encode_symbol_sym32n)
    {
        type = amqp_encode_symbol_sym32n(context, "FooBar", 6);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x05, type->position.index);
        CHECK_EQUAL((size_t) 6, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::foo_bar_symbol_32);
    }

}