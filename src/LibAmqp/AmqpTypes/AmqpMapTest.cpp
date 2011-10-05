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
#include "Context/ContextTestSupport.h"

#include "AmqpTypes/AmqpMultiple.h"

SUITE(AmqpTypes)
{
    class AmqpMapFixture  : public SuiteContext::ContextFixture
    {
    public:
        AmqpMapFixture();
        ~AmqpMapFixture();

    public:
        amqp_multiple_symbol_t *multiple;
    };

    AmqpMapFixture::AmqpMapFixture() : multiple(0)
    {
    }

    AmqpMapFixture::~AmqpMapFixture()
    {
//        amqp_multiple_symbol_free(context, multiple);
    }

    TEST_FIXTURE(AmqpMapFixture, fixture_should_balance_allocations)
    {
//        multiple = amqp_multiple_symbol_create(context, 5);
    }

//    TEST_FIXTURE(AmqpTypes, decode_minimal_frame)
//    {
//        test_data::minimal_frame_header.transfer_to(buffer);
//        frame = amqp_decode_frame(context, buffer);
//        CHECK_EQUAL(8U, frame->data_offset);
//        CHECK_EQUAL(0U, frame->frame_type);
//        CHECK_EQUAL(1U, frame->type_specific.word);
//
//        CHECK_EQUAL(8U, amqp_buffer_index(buffer));
//        CHECK_EQUAL(0, amqp_buffer_available(buffer));
//    }

}
