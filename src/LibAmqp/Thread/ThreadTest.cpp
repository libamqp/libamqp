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
#include "Thread/Thread.h"

SUITE(Thread)
{
    class ThreadFixture
    {
    public:
        ThreadFixture() : saved_from_thread(0)
        {
            amqp_mutex_initialize(&mutex);
            amqp_condition_initialize(&cv);
        }

        ~ThreadFixture()
        {
            amqp_condition_destroy(&cv);
            amqp_mutex_destroy(&mutex);
        }

        const char *saved_from_thread;

        amqp_mutex_t mutex;
        amqp_condition_variable_t cv;

        static const char *pattern;
        static void handler(void *argument);
    private:
    };

    const char *ThreadFixture::pattern = "HeLlO wOrLd";
    void ThreadFixture::handler(void *argument)
    {
        ThreadFixture *fixture = (ThreadFixture *) argument;

        amqp_mutex_lock(&fixture->mutex);
        fixture->saved_from_thread = ThreadFixture::pattern;
        amqp_condition_notify(&fixture->cv);
        amqp_mutex_unlock(&fixture->mutex);
    }

    // Not really proof that the threading wrappers do the right thing.
    TEST_FIXTURE(ThreadFixture, exercise_thread_api)
    {
        CHECK_EQUAL((void *) 0, saved_from_thread);

        amqp_thread_t *thread = amqp_thread_start(ThreadFixture::handler, this);
        amqp_mutex_lock(&mutex);
        while (saved_from_thread == 0)
        {
            amqp_condition_wait(&cv, &mutex);
        }
        amqp_mutex_unlock(&mutex);

        amqp_thread_destroy(thread);

        CHECK_EQUAL(pattern, saved_from_thread);
    }
}
