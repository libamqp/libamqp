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
#include <stdlib.h>

#include "libamqp_common.h"
#include "Thread/Thread.h"

struct amqp_thread_t
{
    amqp_thread_handler_t handler;
    void *argument;
    int running;
    amqp_mutex_t mutex;

#if defined(AMQP__WIN32_THREADS)
    #error "no pthreads"
#else
    pthread_t thread;
#endif
};

typedef void *(*thread_wrapper_t)(void *);

static
void *thread_wrapper(void *argument)
{
    amqp_thread_t *thread = (amqp_thread_t *) argument;
    (*thread->handler)(thread->argument);

    amqp_mutex_lock(&thread->mutex);
    thread->running = false;
    amqp_mutex_unlock(&thread->mutex);

    return 0;
}

static
int create_thread(amqp_thread_t *t, thread_wrapper_t wrapper)
{
#if defined(AMQP__WIN32_THREADS)
    #error "no pthreads"
#else
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

    if (!create_thread(result, thread_wrapper))
    {
        free(result);
        return 0;
    }

    return result;
}

void amqp_thread_destroy(amqp_thread_t *thread)
{
#if defined(AMQP__WIN32_THREADS)
    #error "no pthreads"
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
