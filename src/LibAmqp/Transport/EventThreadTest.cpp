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
#include "Transport/EventThreadTestSupport.h"

#include "Transport/EventThread.h"

SUITE(Transport)
{



    TEST_FIXTURE(EventThreadFixture, event_thread_providing_loop)
    {
        struct ev_loop *loop = ev_default_loop(0);
        m_event_thread = amqp_event_thread_initialize(EventThreadFixture::basic_event_thread_handler, loop);
    }

    TEST_FIXTURE(EventThreadFixture, event_thread_without_loop)
    {
        m_event_thread = amqp_event_thread_initialize(EventThreadFixture::basic_event_thread_handler, 0);
    }


}
