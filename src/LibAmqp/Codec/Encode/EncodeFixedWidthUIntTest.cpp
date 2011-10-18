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
    TEST_FIXTURE(EncodeFixture, EncodeUInt)
    {
        type = amqp_encode_uint(context, buffer, 4294967294U);

        CHECK_NOT_NULL(type);
        CHECK_BUFFERS_MATCH(buffer, test_data::uint_4);

        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x04, type->position.size);
    }

    TEST_FIXTURE(EncodeFixture, EncodeUIntWithZeroValueShouldEncodeUIntZero)
    {
        type = amqp_encode_uint(context, buffer, 0U);

        CHECK_NOT_NULL(type);
        CHECK_BUFFERS_MATCH(buffer, test_data::uint_zero);

        CHECK_EQUAL(0x43, type->format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x00, type->position.size);
    }

    TEST_FIXTURE(EncodeFixture, EncodeUIntWithSmallValueShouldEncodeSmallUint)
    {
        type = amqp_encode_uint(context, buffer, 254U);

        CHECK_NOT_NULL(type);
        CHECK_BUFFERS_MATCH(buffer, test_data::uint_small);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x01, type->position.size);
    }

    TEST_FIXTURE(EncodeFixture, ExplicitEncodeUintZero)
    {
        type = amqp_encode_uint0(context, buffer);

        CHECK_NOT_NULL(type);
        CHECK_BUFFERS_MATCH(buffer, test_data::uint_zero);

        CHECK_EQUAL(0x43, type->format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x00, type->position.size);
    }

    TEST_FIXTURE(EncodeFixture, ExplicitEncodeSmallUInt)
    {
        type = amqp_encode_small_uint(context, buffer, 254U);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x01, type->position.size);
        CHECK_BUFFERS_MATCH(buffer, test_data::uint_small);
    }

    TEST_FIXTURE(EncodeFixture, explicit_encode_small_uint_with_zero_value_should_encode_small_uint_not_uint_zero)
    {
        type = amqp_encode_small_uint(context, buffer, 0U);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL(0x52, type->format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x01, type->position.size);

        CHECK_BUFFERS_MATCH(buffer, test_data::uint_small_zero);
    }
}
