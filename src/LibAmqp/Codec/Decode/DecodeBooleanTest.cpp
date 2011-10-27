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

#include "Codec/Decode/DecodeTestSupport.h"
#include "Codec/Type/Type.h"

SUITE(CodecDecode)
{
    TEST_FIXTURE(DecodeFixture, ZeroByteTrue)
    {
        load_decode_buffer(test_data::true_0);
        type = amqp_decode(context, decode_buffer);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL(0x41, type->constructor.format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x00, type->position.size);
        CHECK(amqp_type_is_not_null(type));
        CHECK(amqp_type_is_boolean(type));

        CHECK(amqp_type_to_boolean(type));
    }

    TEST_FIXTURE(DecodeFixture, ZeroByteFalse)
    {
        load_decode_buffer(test_data::false_0);
        type = amqp_decode(context, decode_buffer);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL(0x42, type->constructor.format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x00, type->position.size);
        CHECK(amqp_type_is_not_null(type));
        CHECK(amqp_type_is_boolean(type));

        CHECK(!amqp_type_to_boolean(type));
    }

    TEST_FIXTURE(DecodeFixture, SingleByteTrue)
    {
        load_decode_buffer(test_data::true_1);
        type = amqp_decode(context, decode_buffer);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL(0x56, type->constructor.format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x01, type->position.size);
        CHECK(amqp_type_is_not_null(type));
        CHECK(amqp_type_is_boolean(type));

        CHECK(amqp_type_to_boolean(type));
    }

    TEST_FIXTURE(DecodeFixture, SingleByteFalse)
    {
        load_decode_buffer(test_data::false_1);
        type = amqp_decode(context, decode_buffer);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL(0x56, type->constructor.format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x01, type->position.size);
        CHECK(amqp_type_is_not_null(type));
        CHECK(amqp_type_is_boolean(type));
        CHECK(!amqp_type_to_boolean(type));
    }

    TEST_FIXTURE(DecodeFixture, DecodeSimpleBooleanArray)
    {
        load_decode_buffer(test_data::boolean_array);
        type = amqp_decode(context, decode_buffer);

        CHECK(check_valid_array());

        CHECK_EQUAL(0x56, type->value.array.elements[0]->constructor.format_code);
        CHECK_EQUAL(2U, type->value.array.count);
        CHECK_EQUAL(2U, type->value.array.count);

        CHECK(amqp_type_is_boolean(type->value.array.elements[0]));
        CHECK(amqp_type_to_boolean(type->value.array.elements[0]));

        CHECK(amqp_type_is_boolean(type->value.array.elements[0]));
        CHECK(!amqp_type_to_boolean(type->value.array.elements[1]));
    }
}
