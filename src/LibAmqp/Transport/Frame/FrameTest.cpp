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

SUITE(Frame)
{
    TEST_FIXTURE(FrameFixture, fixture_should_balance_allocations)
    {
    }

    TEST_FIXTURE(FrameFixture, decode_minimal_frame)
    {
        test_data::minimal_frame_header.transfer_to(buffer);
        frame = amqp_decode_frame(context, buffer);
        CHECK_EQUAL(8U, frame->data_offset);
        CHECK_EQUAL(0U, frame->frame_type);
        CHECK_EQUAL(1U, frame->type_specific.word);

        CHECK_EQUAL(8U, amqp_buffer_index(buffer));
        CHECK_EQUAL(0, amqp_buffer_available(buffer));
    }

    TEST_FIXTURE(FrameFixture, decode_sasl_mechanisms_frame)
    {
        test_data::sasl_mechanisms_frame.transfer_to(buffer);
        frame = amqp_decode_frame(context, buffer);
        CHECK_EQUAL(8U, frame->data_offset);
        CHECK_EQUAL(0U, frame->frame_type);
        CHECK_EQUAL(0U, frame->type_specific.word);


    }
}
