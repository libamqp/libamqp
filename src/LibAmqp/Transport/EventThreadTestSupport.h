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

#ifndef LIBAMQP_TRANSPORT_EVENT_THREAD_TEST_SUPPORT_H
#define LIBAMQP_TRANSPORT_EVENT_THREAD_TEST_SUPPORT_H

#include "Transport/EventThread.h"

namespace SuiteTransport
{
    class EventThreadFixture
    {
    public:
        EventThreadFixture() : m_event_thread(0)
        {
        }
        EventThreadFixture(amqp_event_thread_t *event_thread) : m_event_thread(event_thread)
        {
        }
        ~EventThreadFixture()
        {
            amqp_event_thread_destroy(m_event_thread);
        }

    public:
        amqp_event_thread_t *m_event_thread;
        static void basic_event_thread_handler(amqp_event_thread_t *event_thread);
    };
}

#endif

