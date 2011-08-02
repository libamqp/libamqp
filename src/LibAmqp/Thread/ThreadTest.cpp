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
#include "Thread/FastMutex.h"

#include "debug_helper.h"

SUITE(Thread)
{
    class ThreadFixture
    {
    public:
        ThreadFixture() : saved_from_thread(0)
        {
            amqp_threading_initialize();
            amqp_mutex_initialize(&mutex);
            amqp_condition_initialize(&cv);
        }

        ~ThreadFixture()
        {
            amqp_condition_destroy(&cv);
            amqp_mutex_destroy(&mutex);
            amqp_threading_cleanup();
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

        amqp_mutex_lock(&mutex);
        amqp_thread_t *thread = amqp_thread_start(ThreadFixture::handler, this);
        amqp_condition_wait(&cv, &mutex);
        amqp_mutex_unlock(&mutex);

        amqp_thread_destroy(thread);

        CHECK_EQUAL(pattern, saved_from_thread);
    }


    class FastMutexFixture
    {
    public:
        FastMutexFixture() : count(0), thread_done_count(0), thread_ready_count(0)
        {
            amqp_threading_initialize();
            amqp_fast_mutex_initialize(&fast_mutex);
            amqp_mutex_initialize(&mutex);
            amqp_condition_initialize(&cv);
            amqp_condition_initialize(&cv_gate);
        }

        ~FastMutexFixture()
        {
            amqp_condition_destroy(&cv);
            amqp_condition_destroy(&cv_gate);
            amqp_mutex_destroy(&mutex);
            amqp_fast_mutex_destroy(&fast_mutex);
            amqp_threading_cleanup();
        }

        amqp_fast_mutex_t fast_mutex;
        amqp_mutex_t mutex;
        amqp_condition_variable_t cv;
        amqp_condition_variable_t cv_gate;

        int count;
        int thread_done_count;
        int thread_ready_count;
        static void handler(void *argument);
    private:
    };

#if !defined(LIBAMQP_DISABLE_FAST_MUTEX)
    TEST_FIXTURE(FastMutexFixture, validate_fast_mutex_internal_value)
    {
        CHECK_EQUAL(0, fast_mutex);
        amqp_fast_mutex_lock(&fast_mutex);
        CHECK_EQUAL(1, fast_mutex);
        amqp_fast_mutex_unlock(&fast_mutex);
        CHECK_EQUAL(0, fast_mutex);
    }
#endif

    void FastMutexFixture::handler(void *argument)
    {
        FastMutexFixture *fixture = (FastMutexFixture *) argument;

        amqp_mutex_lock(&fixture->mutex);
        fixture->thread_ready_count++;
        amqp_condition_notify(&fixture->cv);
        amqp_condition_wait(&fixture->cv_gate, &fixture->mutex);
        amqp_mutex_unlock(&fixture->mutex);

        for (int i = 0; i < 10000; i++)
        {
            amqp_fast_mutex_lock(&fixture->fast_mutex);
            fixture->count++;
            amqp_fast_mutex_unlock(&fixture->fast_mutex);
        }

        amqp_mutex_lock(&fixture->mutex);
        fixture->thread_done_count++;
        amqp_condition_notify(&fixture->cv);
        amqp_mutex_unlock(&fixture->mutex);
    }

    TEST_FIXTURE(FastMutexFixture, exercise_thread_api_with_fast_mutex)
    {
        const int N_THREADS = 11;
        amqp_thread_t *threads[N_THREADS];

        for (int i = 0; i < N_THREADS; i++)
        {
            threads[i] = amqp_thread_start(FastMutexFixture::handler, this);
        }

        amqp_mutex_lock(&mutex);
        while (thread_ready_count < N_THREADS)
        {
            amqp_condition_wait(&cv, &mutex);
        }
        amqp_condition_broadcast(&cv_gate);

        while (thread_done_count < N_THREADS)
        {
            amqp_condition_wait(&cv, &mutex);
        }
        amqp_mutex_unlock(&mutex);

        CHECK_EQUAL(N_THREADS * 10000, count);

        for (int i = 0; i < N_THREADS; i++)
        {
            amqp_thread_destroy(threads[i]);
        }
    }
}
