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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_READ_H
#define LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_READ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_CONNECTION_TYPE_T
#define LIBAMQP_AMQP_CONNECTION_TYPE_T
typedef struct amqp_connection_t amqp_connection_t;
#endif

extern void amqp_connection_reader_initialize(amqp_connection_t *connection);
extern void amqp_connection_reader_cleanup(amqp_connection_t *connection);

extern int amqp_connection_reader_is_state(const amqp_connection_t *connection, const char *state_name);

#ifdef __cplusplus
}
#endif
#endif
