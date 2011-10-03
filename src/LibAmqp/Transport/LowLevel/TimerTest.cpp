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
#include "Transport/LowLevel/Timer.h"
#include "debug_helper.h"

SUITE(SlowRunning)
{
    class TimerFixture : public SuiteContext::ContextFixture
    {
    public:
        TimerFixture()
        {
            context->thread_event_loop = ev_default_loop(0);
            timer = amqp_timer_initialize(context, TimerFixture::expiry_handler, 0);
        }
        ~TimerFixture()
        {
            amqp_timer_destroy(context, timer);
        }
        
    public:
        amqp_timer_t *timer;
        static void expiry_handler(amqp_context_t *context, amqp_timer_t *timer);
    };

    void TimerFixture::expiry_handler(amqp_context_t *context, amqp_timer_t *timer)
    {
        ev_break(context->thread_event_loop, EVBREAK_ONE);
    }

    TEST_FIXTURE(TimerFixture, fixture_should_not_cause_an_error)
    {
    }

    TEST_FIXTURE(TimerFixture, event_thread_providing_loop)
    {
        ev_tstamp start = ev_time();
        amqp_timer_start(context, timer, 1.0);
        ev_run(context->thread_event_loop, 0);
        ev_tstamp now = ev_now(context->thread_event_loop);
        CHECK_CLOSE(1.0, now - start, 0.1);
    }
}
