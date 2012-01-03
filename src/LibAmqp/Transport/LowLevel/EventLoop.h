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

#ifndef LIBAMQP_TRANSPORT_LOW_LEVEL_EVENT_LOOP_H
#define LIBAMQP_TRANSPORT_LOW_LEVEL_EVENT_LOOP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ev.h>
#include "Transport/LowLevel/Socket.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_EVENT_LOOP_TYPE_T
#define LIBAMQP_AMQP_EVENT_LOOP_TYPE_T
typedef struct ev_loop amqp_event_loop_t;
#endif

#ifndef LIBAMQP_AMQP_CONNECTION_TYPE_T
#define LIBAMQP_AMQP_CONNECTION_TYPE_T
typedef struct amqp_connection_t amqp_connection_t;
#endif

#ifndef LIBAMQP_AMQP_ACCEPT_HANDLER_ARGUMENTS_TYPE_T
#define LIBAMQP_AMQP_ACCEPT_HANDLER_ARGUMENTS_TYPE_T
typedef struct amqp_accept_handler_arguments_t amqp_accept_handler_arguments_t;
#endif

typedef struct amqp_io_event_watcher_t amqp_io_event_watcher_t;
typedef int (*amqp_accept_event_handle_t)(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t address_size, amqp_accept_handler_arguments_t *arguments);

struct amqp_io_event_watcher_t
{
    ev_io io;
    amqp_context_t *context;
    amqp_event_loop_t *loop;
    union {
        struct {
            amqp_accept_event_handle_t handler;
            amqp_accept_handler_arguments_t *arguments;
        } accept;
        amqp_connection_t *connection;
    } data;
};

typedef void (*amqp_event_handler_t)(amqp_event_loop_t *loop, ev_io* io, const int revents);

// TODO - pass loop
extern void amqp_io_event_watcher_start(amqp_io_event_watcher_t *watcher);
extern void amqp_io_event_watcher_stop(amqp_io_event_watcher_t *watcher);
extern void amqp_io_event_watcher_adjust_priority(amqp_io_event_watcher_t *watcher, int amount);

extern void amqp_io_event_watcher_cb_set(amqp_io_event_watcher_t *watcher, amqp_event_handler_t handler);
extern amqp_io_event_watcher_t *amqp_io_event_watcher_initialize(amqp_context_t *context, amqp_event_loop_t *loop, amqp_event_handler_t handler, int fd, const int revents);
extern void amqp_io_event_watcher_destroy(amqp_context_t *context, amqp_io_event_watcher_t *watcher);

#ifdef __cplusplus
}
#endif
#endif
