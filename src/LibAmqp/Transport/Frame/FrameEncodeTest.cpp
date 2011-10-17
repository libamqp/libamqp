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
#include "Transport/Frame/FrameEncode.h"
#include "Transport/Frame/FrameTestSupport.h"

#include "Codec/Decode/Decode.h"
#include "debug_helper.h"

SUITE(Frame)
{
    TEST_FIXTURE(FrameFixture, sasl_mechanisms_frame_encode)
    {
        amqp_encode_sasl_frame(context, buffer);

        frame = amqp_decode_sasl_frame(context, buffer);
        ASSERT(frame != 0);

        CHECK_EQUAL(8U, frame->data_offset);
        CHECK_EQUAL(AMQP_SASL_FRAME_TYPE, frame->frame_type);
        CHECK_EQUAL(0U, frame->type_specific.word);

        amqp_multiple_symbol_t *multiple = &frame->frames.sasl.mechanisms.sasl_server_mechanisms;
        CHECK(amqp_symbol_compare_with_cstr(amqp_multiple_symbol_get(multiple, 0), "PLAIN") == 0);
    }
}
