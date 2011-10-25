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

#ifndef LIBAMQP_TRANSPORT_LOW_LEVEL_LISTEN_THREAD_H
#define LIBAMQP_TRANSPORT_LOW_LEVEL_LISTEN_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Transport/LowLevel/EventLoop.h"
#include "Transport/AcceptHandler.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_EVENT_THREAD_TYPE_T
#define LIBAMQP_AMQP_EVENT_THREAD_TYPE_T
typedef struct amqp_event_thread_t amqp_event_thread_t;
#endif

typedef struct amqp_listener_thread_t
{
    amqp_event_thread_t *thread;
    int port_number;
    amqp_accept_event_handle_t accept_handler;
    amqp_accept_handler_arguments_t *accept_handler_arguments;
    const char *context_name;
} amqp_listener_thread_t;

extern amqp_listener_thread_t *amqp_listener_thread_initialize(amqp_context_t *context, amqp_event_loop_t *loop, int listen_port_number, amqp_accept_event_handle_t accept_handler, const char *context_name, amqp_accept_handler_arguments_t *arguments);
extern void amqp_listener_thread_destroy(amqp_context_t *context, amqp_listener_thread_t *broker);

#ifdef __cplusplus
}
#endif
#endif
