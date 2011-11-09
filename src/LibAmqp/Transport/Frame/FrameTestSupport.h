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

#ifndef LIBAMQP_TRANSPORT_FRAME_FRAME_TEST_SUPPORT_H
#define LIBAMQP_TRANSPORT_FRAME_FRAME_TEST_SUPPORT_H

#include "libamqp_common.h"
#include "Context/ContextTestSupport.h"
#include "Transport/Connection/ConnectionTestSupport.h"
#include "Transport/Frame/Frame.h"
#include "TestData/TestFrames.h"

namespace SuiteFrame
{
    class FrameFixture : public SuiteConnection::BaseConnectionFixture
    {
    public:
        FrameFixture();
        ~FrameFixture();

    int check_header(uint8_t frame_type, uint16_t frame_specific);
    int check_sasl_header();
    int check_amqp_header();

    public:
        amqp_frame_t *frame;
        amqp_type_t *type;
    };
}
#endif
