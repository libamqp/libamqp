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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_SESSION_H
#define LIBAMQP_TRANSPORT_CONNECTION_SESSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "libamqp.h"

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_CONNECTION_TYPE_T
#define LIBAMQP_AMQP_CONNECTION_TYPE_T
typedef struct amqp_connection_t amqp_connection_t;
#endif

#ifndef LIBAMQP_AMQP_SESSION_TYPE_T
#define LIBAMQP_AMQP_SESSION_TYPE_T
typedef struct amqp_session_t amqp_session_t;
#endif

#ifndef LIBAMQP_AMQP_CAPABILITIES_TYPE_T
#define LIBAMQP_AMQP_CAPABILITIES_TYPE_T
typedef struct amqp_capabilities_t amqp_capabilities_t;
#endif

#ifndef LIBAMQP_AMQP_PROPERTIES_TYPE_T
#define LIBAMQP_AMQP_PROPERTIES_TYPE_T
typedef struct amqp_properties_t amqp_properties_t;
#endif

typedef struct amqp_session_values_t
{
    uint16_t channel;
    uint32_t next_outgoing_id;
    uint32_t incomming_window;
    uint32_t outgoing_window;
    int32_t handle_max;
} amqp_session_values_t;

struct amqp_session_t
{
    amqp_connection_t *connection;
    amqp_session_values_t local;
    int remote_defined;
    amqp_session_values_t remote;
    amqp_capabilities_t *offered_capabilities;
    amqp_capabilities_t *desired_capabilities;
    amqp_properties_t *properties;
};

typedef struct amqp_sessions_table_t
{
    amqp_session_t *sessions[AMQP_DEFAULT_CHANNEL_MAX];
} amqp_sessions_table_t;

extern amqp_session_t *amqp_session_create(amqp_connection_t *connection);
extern int amqp_connection_next_channel(amqp_connection_t *connection);
extern void amqp_session_initialize(amqp_connection_t *connection, amqp_session_t *session, int channel);
extern void amqp_session_cleanup(amqp_session_t *session);

#ifdef __cplusplus
}
#endif
#endif
