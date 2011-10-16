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
        ASSERT_BUFFERS_MATCH(buffer, test_data::true_0);

        CHECK_EQUAL(0x41, type->format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x00, type->position.size);
        CHECK_EQUAL(0U, type->flags.is_null);

        // TODO - initialise value of encoded type
        CHECK_EQUAL(0U, type->value.b1._unsigned);
    }

    TEST_FIXTURE(EncodeFixture, EncodeFalse)
    {
        type = amqp_encode_boolean(context, buffer, 0);

        CHECK_NOT_NULL(type);
        ASSERT_BUFFERS_MATCH(buffer, test_data::false_0);

        CHECK_EQUAL(0x42, type->format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x00, type->position.size);
        CHECK_EQUAL(0U, type->flags.is_null);

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
        CHECK_EQUAL(0U, type->flags.is_invalid);
        CHECK_EQUAL(1U, type->flags.container.type.is_array);

        amqp_buffer_put_buffer_contents(context->decode.buffer, buffer);
        deallocate_type(type);
        type = amqp_decode(context, context->decode.buffer);
        CHECK(check_valid_array());

        CHECK(amqp_convert_to_boolean(type->value.array.elements[0]));
        CHECK(!amqp_convert_to_boolean(type->value.array.elements[1]));
        CHECK(!amqp_convert_to_boolean(type->value.array.elements[2]));
        CHECK(amqp_convert_to_boolean(type->value.array.elements[3]));
        CHECK(amqp_convert_to_boolean(type->value.array.elements[4]));
        CHECK(amqp_convert_to_boolean(type->value.array.elements[5]));
        CHECK(!amqp_convert_to_boolean(type->value.array.elements[6]));
    }
}
