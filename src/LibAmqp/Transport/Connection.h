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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_H
#define LIBAMQP_TRANSPORT_CONNECTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Transport/LowLevel/Connect.h"
#include "Transport/LowLevel/Timer.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_CONNECTION_TYPE_T
#define LIBAMQP_AMQP_CONNECTION_TYPE_T
typedef struct amqp_connection_t amqp_connection_t;
#endif

typedef struct amqp_connection_state_t amqp_connection_state_t;

typedef void (*amqp_connection_stop_f)(amqp_context_t *context, amqp_connection_t *connection);
typedef void (*amqp_connection_connect_f)(amqp_context_t *context, amqp_connection_t *connection, const char *hostname, int port);
typedef void (*amqp_connection_error_f)(amqp_context_t *context, amqp_connection_t *connection, int error_code);
typedef void (*amqp_connection_next_f)(amqp_context_t *context, amqp_connection_t *connection);
typedef void (*amqp_connection_io_f)(amqp_context_t *context, amqp_connection_t *connection, amqp_event_loop_t *loop, amqp_io_event_watcher_t *watcher);
typedef void (*amqp_connection_finish_f)(amqp_context_t *context, amqp_connection_t *connection);

struct amqp_connection_state_t
{
    const char *name;
    amqp_connection_stop_f shutdown;
    amqp_connection_connect_f connect;
    amqp_connection_error_f lookup_failed;
    amqp_connection_error_f connect_failed;
    amqp_connection_next_f next;
    amqp_connection_finish_f connect_finish;
//    amqp_connection_io_f read;
//    amqp_connection_io_f write;
};

struct amqp_connection_t
{
    amqp_connection_state_t state;
    amqp_timer_t *inactivity_timer;
    struct addrinfo *iterator;
    struct addrinfo *addrinfo;
    int eai_error_code;
    int errno_error_code;
    int running;
    int socket_fd;
    amqp_io_event_watcher_t *read_watcher;
    amqp_io_event_watcher_t *write_watcher;
    const char *hostname;
    int port_number;
};

extern amqp_connection_t *amqp_connection_initialize(amqp_context_t *context);
extern void amqp_connection_destroy(amqp_context_t *context, amqp_connection_t *connection);

extern void amqp_connection_state_initialize(amqp_context_t *context, amqp_connection_t *connection);
extern void amqp_connection_connect(amqp_context_t *context, amqp_connection_t *connection, const char *hostname, int port_number);
extern int amqp_connection_is_state(amqp_connection_t *connection, const char *state);

#ifdef __cplusplus
}
#endif
#endif
