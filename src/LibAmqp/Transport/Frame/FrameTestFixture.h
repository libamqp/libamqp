/*
   Copyright 2011-2012 StormMQ Limited

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
#include "Context/ContextTestFixture.h"
#include "Transport/Frame/Frame.h"
#include "TestData/TestFrames.h"

#include "Transport/Connection/TestSupport/ConnectionHolder.h"
#include "Context/TestSupport/DecodeBufferHolder.h"
#include "Plugin/TestSupport/SaslPluginHolder.h"
#include "Transport/Connection/TestSupport/WriteInterceptor.h"
#include "Context/TestSupport/TypeHolder.h"
#include "Transport/Frame/TestSupport/FrameHolder.h"

namespace SuiteFrame
{
    class FrameTestFixture :
            public TestSupport::UnconnectedConnectionHolder,
            public virtual TestSupport::DecodeBufferHolder,
            public virtual TestSupport::SaslPluginHolder,
            public virtual TestSupport::WriteInterceptor,
            public virtual TestSupport::TypeHolder,
            public virtual TestSupport::FrameHolder
    {
    public:
        FrameTestFixture();
        ~FrameTestFixture();

        int check_header(uint8_t frame_type, uint16_t frame_specific);
        int check_sasl_header();
        int check_amqp_header();
    };
}
#endif
