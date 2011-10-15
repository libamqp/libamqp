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
#include "Transport/Frame/FrameTestSupport.h"
#include "debug_helper.h"

namespace SuiteFrame
{
    FrameFixture::FrameFixture() : frame(0), type(0)
    {
        buffer = amqp_allocate_buffer(context);
    }

    FrameFixture::~FrameFixture()
    {
        amqp_deallocate_buffer(context, buffer);
        amqp_frame_cleanup(context, frame);
        amqp_deallocate_type(context, type);
    }
}
