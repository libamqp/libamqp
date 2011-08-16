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

#ifndef LIBAMQP_TRANSPORT_EVENT_THREAD_H
#define LIBAMQP_TRANSPORT_EVENT_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Thread/Thread.h"
#include "Transport/EventLoop.h"

#ifndef LIBAMQP_AMQP_EVENT_THREAD_TYPE_T
#define LIBAMQP_AMQP_EVENT_THREAD_TYPE_T
typedef struct amqp_event_thread_t amqp_event_thread_t;
#endif

typedef void (*amqp_event_thread_handler_t)(amqp_event_thread_t *thread);

struct amqp_event_thread_t
{
    amqp_semaphore_t thread_running_semaphore;
    amqp_event_thread_handler_t handler;
    amqp_event_loop_t *loop;
    amqp_context_t *context;
    amqp_thread_t *thread;
    ev_async async_watcher;
};

extern amqp_event_thread_t *amqp_event_thread_initialize(amqp_event_thread_handler_t handler, amqp_context_t *context, amqp_event_loop_t *loop);
extern void amqp_event_thread_destroy(amqp_event_thread_t *event_thread);
extern void amqp_event_thread_run_loop(amqp_event_thread_t *event_thread);

#ifdef __cplusplus
}
#endif
#endif
