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

#ifndef LIBAMQP_TRANSPORT_TRANSPORT_H
#define LIBAMQP_TRANSPORT_TRANSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ev.h>

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_TRANSPORT_STATE_TYPE_T
#define LIBAMQP_AMQP_TRANSPORT_STATE_TYPE_T
typedef struct amqp_transport_state_t amqp_transport_state_t;
#endif

typedef struct amqp_endpoint_address_t
{
    const char *hostname;
    int port;
} amqp_endpoint_address_t;

typedef struct amqp_connection_t amqp_connection_t;

typedef void (*amqp_connection_callback_t)(amqp_connection_t *connection);

typedef struct amqp_endpoint_fn_table_t amqp_endpoint_fn_table_t;
typedef struct amqp_endpoint_t amqp_endpoint_t;
struct amqp_endpoint_t
{
    amqp_endpoint_fn_table_t *functions;
//    amqp_endpoint_address_t address;
//    int (*read)(unsigned char *buffer, size_t bufsiz, size_t offset, int n, amqp_connection_callback_t callback);
//    int (*write)(unsigned char *buffer, int n, amqp_connection_callback_t callback);
};

struct amqp_connection_t
{
    amqp_context_t *context;
    amqp_endpoint_t *endpoint;
};

struct amqp_transport_state_t
{
    struct ev_loop *loop;
};

extern amqp_transport_state_t * amqp_transport_initialize_with_ev_loop(amqp_context_t *context, struct ev_loop *loop);
extern void amqp_transport_cleanup(amqp_transport_state_t *transport_state);

extern amqp_endpoint_t *amqp__endpoint_initialize(amqp_context_t *context, amqp_endpoint_address_t *address);
extern void amqp_endpoint_destroy(amqp_endpoint_t *endpoint);

extern amqp_connection_t *amqp__create_connection(amqp_context_t *context, amqp_endpoint_t *endpoint, amqp_connection_callback_t callback);

#ifdef __cplusplus
}
#endif
#endif
