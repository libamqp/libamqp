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

#include "Codec/Encode/EncodeTestSupport.h"

SUITE(CodecEncode)
{
    TEST_FIXTURE(EncodeFixture, EncodeUShort)
    {
        type = amqp_encode_ushort(context, 65534);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x02, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::ushort_2);
    }

    TEST_FIXTURE(EncodeFixture, EncodeShort)
    {
        type = amqp_encode_short(context, -2);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x02, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::short_2);
    }

    TEST_FIXTURE(EncodeFixture, EncodeUByte)
    {
        type = amqp_encode_ubyte(context, 254);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x01, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::ubyte_1);
    }

    TEST_FIXTURE(EncodeFixture, EncodeByte)
    {
        type = amqp_encode_byte(context, -2);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x01, type->position.size);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::byte_1);
    }


    TEST_FIXTURE(EncodeFixture, TimeStamp)
    {
        type = amqp_encode_timestamp(context, 1291654800000LL);

        CHECK_NOT_NULL(type);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::timestamp_8);

        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x08, type->position.size);
    }

    TEST_FIXTURE(EncodeFixture, TimeStamp2)
    {
        type = amqp_encode_timestamp(context, -1864105200000LL);

        CHECK_NOT_NULL(type);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::timestamp_before_epoc_8);

        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x08, type->position.size);
    }

    TEST_FIXTURE(EncodeFixture, Float)
    {
        type = amqp_encode_float(context, 123.456f);

        CHECK_NOT_NULL(type);

        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x04, type->position.size);

        amqp_buffer_put_buffer_contents(context->decode.buffer, context->encode.buffer);

        result = amqp_decode(context, context->decode.buffer);
        CHECK_NOT_NULL(result);
        ASSERT_VALID(result);
        CHECK_CLOSE(123.456f, result->value.b4._float, 0.00001);
    }

    TEST_FIXTURE(EncodeFixture, Double)
    {
        type = amqp_encode_double(context, 123.456);

        CHECK_NOT_NULL(type);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::double_8);

        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x08, type->position.size);

        amqp_buffer_put_buffer_contents(context->decode.buffer, context->encode.buffer);

        result = amqp_decode(context, context->decode.buffer);
        CHECK_NOT_NULL(result);
        ASSERT_VALID(result);
        CHECK_CLOSE(123.456, result->value.b8._double, 0.00001);
    }

    TEST_FIXTURE(EncodeFixture, uuid)
    {
        amqp_uuid_t uuid = {0xf8, 0x1d, 0x4f, 0xae, 0x7d, 0xec, 0x11, 0xd0, 0xa7, 0x65, 0x00, 0xa0, 0xc9, 0x1e, 0x6b, 0xf6};
        type = amqp_encode_uuid(context, &uuid);

        CHECK_NOT_NULL(type);
        ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::uuid_16);

        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x10, type->position.size);
    }

    TEST_FIXTURE(EncodeFixture, uuid_t_width)
    {
        CHECK_EQUAL((size_t) 16, sizeof(amqp_uuid_t));
    }

}
