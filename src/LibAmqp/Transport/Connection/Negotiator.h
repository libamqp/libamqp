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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_NEGOTIATOR_H
#define LIBAMQP_TRANSPORT_CONNECTION_NEGOTIATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "libamqp_common.h"

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

#define AMQP_PROTOCOL_VERSION(id, major, minor, release)  ((id << 24) | (major << 16) | (minor << 8) | release)


extern void amqp_negotiator_encode_version(amqp_context_t *context, uint32_t version, amqp_buffer_t *buffer);
extern uint32_t amqp_negotiator_decode_version(amqp_context_t *context, amqp_buffer_t *buffer);

extern uint32_t amqp_negotiator_choose_amqp_protocol_version(amqp_connection_t *connection, int requested_version);
extern uint32_t amqp_negotiator_choose_sasl_protocol_version(amqp_connection_t *connection, int requested_version);
extern uint32_t amqp_negotiator_choose_tls_protocol_version(amqp_connection_t *connection, int requested_version);

inline static
int amqp_version_protocol_id(uint32_t version)
{
    return (version >> 24) & 0xff;
}

inline static
int amqp_version_major(uint32_t version)
{
    return (version >> 16) & 0xff;
}

inline static
int amqp_version_minor(uint32_t version)
{
    return (version >> 8) & 0xff;
}

inline static
int amqp_version_release(uint32_t version)
{
    return version & 0xff;
}

#ifdef __cplusplus
}
#endif
#endif
