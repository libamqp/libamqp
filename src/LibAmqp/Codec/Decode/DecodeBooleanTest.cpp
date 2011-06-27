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
    TEST_FIXTURE(DecodeFixture, True)
    {
        load_decode_buffer(test_data::true_0);
        type = amqp_decode(context);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL(0x41, type->format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x00, type->position.size);
        CHECK_EQUAL(0U, type->flags.is_null);

        CHECK(amqp_convert_to_boolean(type));
    }

    TEST_FIXTURE(DecodeFixture, False)
    {
        load_decode_buffer(test_data::false_0);
        type = amqp_decode(context);

        CHECK_NOT_NULL(type);
        CHECK_EQUAL(0x42, type->format_code);
        CHECK_EQUAL((size_t) 0x01, type->position.index);
        CHECK_EQUAL((size_t) 0x00, type->position.size);
        CHECK_EQUAL(0U, type->flags.is_null);

        CHECK(!amqp_convert_to_boolean(type));
    }
}
