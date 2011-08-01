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
#include <assert.h>
#include <stdlib.h>

#include "libamqp_common.h"
#include "Thread/Thread.h"

#if defined(AMQP__WIN32_THREADS)
void amqp_threading_initialize()
{
}
void amqp_threading_cleanup()
{
}
#else
static amqp_mutex_t global_mutex;
static volatile unsigned int next_thread_id = 1;
static volatile int threading_initialized = 0;

void amqp_threading_initialize()
{
    threading_initialized = 1;
    amqp_mutex_initialize(&global_mutex);
}
void amqp_threading_cleanup()
{
    amqp_mutex_destroy(&global_mutex);
}
unsigned int get_next_posix_thread_id()
{
    int result;
    assert(threading_initialized);

    amqp_mutex_lock(&global_mutex);
    result = next_thread_id++;
    amqp_mutex_unlock(&global_mutex);

    return result;
}
#endif

struct amqp_thread_t
{
    amqp_thread_handler_t handler;
    void *argument;
    int running;
    amqp_mutex_t mutex;
    unsigned int thread_id;

#if defined(AMQP__WIN32_THREADS)
    HANDLE thread;
#else
    pthread_t thread;
#endif
};

#if defined(AMQP__WIN32_THREADS)
#define WRAPPER_RETURN_TYPE unsigned WINAPI
#else
#define WRAPPER_RETURN_TYPE void *
#endif

static
WRAPPER_RETURN_TYPE thread_wrapper(void *argument)
{
    amqp_thread_t *thread = (amqp_thread_t *) argument;
    (*thread->handler)(thread->argument);

    amqp_mutex_lock(&thread->mutex);
    thread->running = false;
    amqp_mutex_unlock(&thread->mutex);

    return 0;
}

static
int create_thread(amqp_thread_t *t)
{
#if defined(AMQP__WIN32_THREADS)
    t->thread = CreateThread(
            NULL,              // default security
            0,                 // default stack size
            thread_wrapper,    // name of the thread function
            (void *) t,        // no thread parameters
            0,                 // default startup flags
            &t->thread_id);
    return t->thread != 0;
#else
    t->thread_id = get_next_posix_thread_id();
    return pthread_create(&t->thread, 0, wrapper, t) == 0;
#endif
}

amqp_thread_t *amqp_thread_start(amqp_thread_handler_t handler, void *handler_argument)
{
    amqp_thread_t *result = (amqp_thread_t *) malloc(sizeof(amqp_thread_t));
    result->handler = handler;
    result->argument = handler_argument;
    result->running = true;

    amqp_mutex_initialize(&result->mutex);

    if (!create_thread(result))
    {
        free(result);
        return 0;
    }

    return result;
}

void amqp_thread_destroy(amqp_thread_t *thread)
{
#if defined(AMQP__WIN32_THREADS)
    WaitForSingleObject(thread->thread, INFINITE);
#else
    pthread_join(thread->thread, 0);
#endif

    amqp_mutex_destroy(&thread->mutex);
    free(thread);
}

int amqp_thread_is_running(amqp_thread_t *thread)
{
    int result;
    amqp_mutex_lock(&thread->mutex);
    result = thread->running;
    amqp_mutex_unlock(&thread->mutex);
    return result;
}
