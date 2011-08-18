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

#ifndef LIBAMQP_TRANSPORT_EVENT_LOOP_H
#define LIBAMQP_TRANSPORT_EVENT_LOOP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ev.h>
#include "Transport/Socket.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

typedef struct ev_loop amqp_event_loop_t;
typedef struct amqp_io_event_watcher_t amqp_io_event_watcher_t;

typedef struct amqp_event_fn_list_t {
    int (*accept)(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t adress_size);
} amqp_event_fn_list_t;
struct amqp_io_event_watcher_t
{
    ev_io io;
    amqp_context_t *context;
    amqp_event_loop_t *loop;
    amqp_event_fn_list_t *fns;
};

typedef void (*amqp_event_handler_t)(amqp_event_loop_t *loop, ev_io* io, const int revents);

extern void amqp_io_event_watcher_start(amqp_io_event_watcher_t *watcher);
extern void amqp_io_event_watcher_stop(amqp_io_event_watcher_t *watcher);
extern void amqp_io_event_watcher_adjust_priority(amqp_io_event_watcher_t *watcher, int amount);

extern amqp_io_event_watcher_t *amqp_io_event_watcher_initialize(amqp_context_t *context, amqp_event_loop_t *loop, amqp_event_handler_t handler, int fd, const int revents);
extern void amqp_io_event_watcher_destroy(amqp_context_t *context, amqp_io_event_watcher_t *watcher);

#ifdef __cplusplus
}
#endif
#endif
