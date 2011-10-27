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

#include "debug_helper.h"

SUITE(CodecEncode)
{
    TEST_FIXTURE(EncodeFixture, EncodeTrue)
    {
        type = amqp_encode_boolean(context, buffer, 1);

        CHECK_NOT_NULL(type);
        CHECK_BUFFERS_MATCH(buffer, test_data::true_0);

        CHECK_EQUAL(0x41, type->format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x00, type->position.size);
        CHECK(amqp_type_is_not_null(type));
        CHECK(amqp_type_is_boolean(type));
        CHECK(amqp_type_to_boolean(type));
    }

    TEST_FIXTURE(EncodeFixture, EncodeFalse)
    {
        type = amqp_encode_boolean(context, buffer, 0);

        CHECK_NOT_NULL(type);
        CHECK_BUFFERS_MATCH(buffer, test_data::false_0);

        CHECK_EQUAL(0x42, type->format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x00, type->position.size);
        CHECK(amqp_type_is_not_null(type));
        CHECK(amqp_type_is_boolean(type));
        CHECK(!amqp_type_to_boolean(type));
    }

    TEST_FIXTURE(EncodeFixture, EncodeArrayOfBoolean)
    {
        type = amqp_encode_array_8(context, buffer);
            amqp_encode_boolean(context, buffer, true);
            amqp_encode_boolean(context, buffer, false);
            amqp_encode_boolean(context, buffer, false);
            amqp_encode_boolean(context, buffer, true);
            amqp_encode_boolean(context, buffer, true);
            amqp_encode_boolean(context, buffer, true);
            amqp_encode_boolean(context, buffer, false);
        CHECK_NOT_NULL(type);
        amqp_complete_type(context, buffer, type);
        break_one();

        CHECK(amqp_type_is_valid(type));
        CHECK(amqp_type_is_array(type));
        CHECK_EQUAL(0x56, type->value.array.elements[0]->format_code);
    }

    TEST_FIXTURE(EncodeFixture, EncodeAndDecodeArrayOfBoolean)
    {
        type = amqp_encode_array_8(context, buffer);
            amqp_encode_boolean(context, buffer, true);
            amqp_encode_boolean(context, buffer, false);
            amqp_encode_boolean(context, buffer, false);
            amqp_encode_boolean(context, buffer, true);
            amqp_encode_boolean(context, buffer, true);
            amqp_encode_boolean(context, buffer, true);
            amqp_encode_boolean(context, buffer, false);
        CHECK_NOT_NULL(type);
        amqp_complete_type(context, buffer, type);

        CHECK(amqp_type_is_valid(type));
        CHECK(amqp_type_is_array(type));

        deallocate_type(type);

        type = amqp_decode(context, buffer);
        CHECK(check_valid_array());

        CHECK_EQUAL(0xe0, type->format_code);
        CHECK_EQUAL(0x56, type->value.array.elements[0]->format_code);

        CHECK(amqp_type_to_boolean(type->value.array.elements[0]));
        CHECK(!amqp_type_to_boolean(type->value.array.elements[1]));
        CHECK(!amqp_type_to_boolean(type->value.array.elements[2]));
        CHECK(amqp_type_to_boolean(type->value.array.elements[3]));
        CHECK(amqp_type_to_boolean(type->value.array.elements[4]));
        CHECK(amqp_type_to_boolean(type->value.array.elements[5]));
        CHECK(!amqp_type_to_boolean(type->value.array.elements[6]));
    }
}
