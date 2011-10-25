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

#ifndef LIBAMQP_TRANSPORT_LOW_LEVEL_EVENT_THREAD_H
#define LIBAMQP_TRANSPORT_LOW_LEVEL_EVENT_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Thread/Thread.h"
#include "Transport/LowLevel/EventLoop.h"

#ifndef LIBAMQP_AMQP_EVENT_THREAD_TYPE_T
#define LIBAMQP_AMQP_EVENT_THREAD_TYPE_T
typedef struct amqp_event_thread_t amqp_event_thread_t;
#endif

#ifndef LIBAMQP_AMQP_ACCEPT_HANDLER_ARGUMENTS_TYPE_T
#define LIBAMQP_AMQP_ACCEPT_HANDLER_ARGUMENTS_TYPE_T
typedef struct amqp_accept_handler_arguments_t amqp_accept_handler_arguments_t;
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
    void *thread_arguments;
};

extern amqp_event_thread_t *amqp_event_thread_initialize(amqp_context_t *context, amqp_event_thread_handler_t handler, amqp_event_loop_t *loop, void *argument, const char *context_name);
extern int amqp_event_thread_destroy(amqp_context_t *context, amqp_event_thread_t *event_thread);

typedef void (*amqp_connection_shutdown_hook_t)(amqp_context_t *context, amqp_event_loop_t *loop, amqp_accept_handler_arguments_t *argument);
extern void amqp_event_thread_run_loop_with_shutdown_hook(amqp_event_thread_t *event_thread, amqp_connection_shutdown_hook_t hook, amqp_accept_handler_arguments_t *argument);
extern void amqp_event_thread_run_loop(amqp_event_thread_t *event_thread);

#ifdef __cplusplus
}
#endif
#endif
