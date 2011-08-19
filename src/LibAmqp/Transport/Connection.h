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

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

typedef struct amqp_connection_state_t amqp_connection_state_t;

typedef void (*amqp_connection_stop_f)(amqp_context_t *context, amqp_connection_state_t *state);
typedef void (*amqp_connection_start_f)(amqp_context_t *context, amqp_connection_state_t *state);

struct amqp_connection_state_t
{
    const char *name;
    amqp_connection_stop_f stop;
    amqp_connection_start_f start;
};

typedef struct amqp_connection_t
{
    amqp_connection_state_t state;
} amqp_connection_t;

extern amqp_connection_t *amqp_connection_initialize(amqp_context_t *context);
extern void amqp_connection_destroy(amqp_context_t *context, amqp_connection_t *connection);

#ifdef __cplusplus
}
#endif
#endif
