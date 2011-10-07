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
#include "Transport/Frame/Frame.h"
#include "Transport/Frame/FrameTestSupport.h"

#include "Codec/Decode/Decode.h"
#include "debug_helper.h"

SUITE(Frame)
{
    TEST_FIXTURE(FrameFixture, multiple_symbol_null)
    {
        test_data::multiple_symbol_null.transfer_to(buffer);
        type = amqp_decode(context, buffer);

        CHECK(amqp_type_is_null(type));
    }

    TEST_FIXTURE(FrameFixture, multiple_symbol_one_value)
    {
        test_data::multiple_symbol_one_value.transfer_to(buffer);
        type = amqp_decode(context, buffer);

        CHECK(amqp_type_is_symbol(type));
    }

    TEST_FIXTURE(FrameFixture, multiple_symbol_many_values)
    {
        test_data::multiple_symbol_many_values.transfer_to(buffer);
        type = amqp_decode(context, buffer);

        CHECK(amqp_type_is_array(type));
        CHECK_EQUAL(3U, type->value.array.count);
        CHECK(amqp_type_is_symbol(type->value.array.elements[0]));
    }


}
