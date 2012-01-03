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

#include <TestHarness.h>
#include "Thread/Thread.h"

SUITE(Thread)
{
    class SemaphoreFixture
    {
    public:
        SemaphoreFixture() : m_saved_from_thread(0)
        {
            amqp_semaphore_initialize(&m_semaphore);
        }

        ~SemaphoreFixture()
        {
            amqp_semaphore_destroy(&m_semaphore);
        }

        amqp_semaphore_t m_semaphore;
        const char *m_saved_from_thread;

        static const char *pattern;
        static void handler(void *argument);
    private:
    };

    const char *SemaphoreFixture::pattern = "hElLo WoRlD";

    void SemaphoreFixture::handler(void *argument)
    {
        SemaphoreFixture *fixture = (SemaphoreFixture *) argument;

        fixture->m_saved_from_thread = SemaphoreFixture::pattern;
        amqp_semaphore_signal(&fixture->m_semaphore);
    }

    TEST_FIXTURE(SemaphoreFixture, exercise_thread_api)
    {
        CHECK_EQUAL((void *) 0, m_saved_from_thread);

        amqp_thread_t *thread = amqp_thread_start(SemaphoreFixture::handler, this);
        amqp_semaphore_wait(&m_semaphore);
        amqp_thread_destroy(thread);

        CHECK_EQUAL(SemaphoreFixture::pattern, m_saved_from_thread);
    }
}
