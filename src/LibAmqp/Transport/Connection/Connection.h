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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_STATE_H
#define LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Transport/LowLevel/Connect.h"
#include "Transport/LowLevel/Timer.h"
#include "Transport/Connection/ConnectionState.h"
#include "Transport/Connection/Negotiator.h"

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

#ifndef LIBAMQP_AMQP_CONNECTIONS_TYPE_T
#define LIBAMQP_AMQP_CONNECTIONS_TYPE_T
typedef struct amqp_connections_t amqp_connections_t;
#endif

#ifndef LIBAMQP_AMQP_ACCEPT_HANDLER_ARGUMENTS_TYPE_T
#define LIBAMQP_AMQP_ACCEPT_HANDLER_ARGUMENTS_TYPE_T
typedef struct amqp_accept_handler_arguments_t amqp_accept_handler_arguments_t;
#endif

#define AMQP_PREFERRED_VERSION      AMQP_PROTOCOL_VERSION(AMQP_PROTOCOL_ID, AMQP_PROTOCOL_MAJOR, AMQP_PROTOCOL_MINOR, AMQP_PROTOCOL_REVISION)
#define AMQP_TLS_PREFERRED_VERSION      AMQP_PROTOCOL_VERSION(AMQP_TLS_PROTOCOL_ID, AMQP_TLS_PROTOCOL_MAJOR, AMQP_TLS_PROTOCOL_MINOR, AMQP_TLS_PROTOCOL_REVISION)
#define AMQP_SASL_PREFERRED_VERSION     AMQP_PROTOCOL_VERSION(AMQP_SASL_PROTOCOL_ID, AMQP_SASL_PROTOCOL_MAJOR, AMQP_SASL_PROTOCOL_MINOR, AMQP_SASL_PROTOCOL_REVISION)

enum amqp_connection_flags
{
    AMQP_CONNECTION_RUNNING = 0x01,
    AMQP_CONNECTION_SOCKET_CONNECTED = 0x02,
    AMQP_CONNECTION_SOCKET_ACCEPTED = 0x04,
    AMQP_CONNECTION_TLS_CONNECTED = 0x08,
    AMQP_CONNECTION_SASL_CONNECTED = 0x10,
    AMQP_CONNECTION_AMQP_CONNECTED = 0x20
};
enum amqp_connection_failure_flags
{
    AMQP_CONNECTION_PIPE = 0x01,
    AMQP_CONNECTION_SOCKET_CONNECT_FAILED = 0x02,
    AMQP_CONNECTION_TLS_NEGOTIATION_REJECTED = 0x04,
    AMQP_CONNECTION_SASL_NEGOTIATION_REJECTED = 0x08,
    AMQP_CONNECTION_AMQP_NEGOTIATION_REJECTED = 0x10,
    AMQP_CONNECTION_READ_ERROR = 0x20,
    AMQP_CONNECTION_WRITE_ERROR = 0x40,
    AMQP_CONNECTION_READ_EOF = 0x80,
    AMQP_CONNECTION_TLS_ERROR = 0x0100,
    AMQP_CONNECTION_SASL_ERROR = 0x0200,
    AMQP_CONNECTION_AMQP_ERROR = 0x0400,
    AMQP_CONNECTION_SASL_FRAME_DECODE_ERROR = 0x0800,
    AMQP_CONNECTION_AMQP_FRAME_DECODE_ERROR = 0x1000,
    AMQP_CONNECTION_FRAME_SEQUENCE_ERROR = 0x2000,

};

extern amqp_connection_t *amqp_connection_create(amqp_context_t *context);
extern void amqp_connection_destroy(amqp_context_t *context, amqp_connection_t *connection);

extern void amqp_connection_connect(amqp_context_t *context, amqp_connection_t *connection);
extern void amqp_connection_connect_to(amqp_context_t *context, amqp_connection_t *connection, const char *hostname, int port_number);
extern amqp_connection_t *amqp_connection_accept(amqp_context_t *context, int fd, struct sockaddr_storage *client_address, socklen_t address_size, amqp_accept_handler_arguments_t *arguments);

extern void amqp_connection_shutdown_confirm(amqp_context_t *context, amqp_connection_t *connection);

extern void amqp_connection_shutdown(amqp_context_t *context, amqp_connection_t *connection);

extern void amqp_connection_write(amqp_context_t *context, amqp_connection_t *connection, amqp_buffer_t *buffer);
extern void amqp_connection_read(amqp_context_t *context, amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required);

inline static int amqp_connection_is(const amqp_connection_t *connection, int flags)
{
    return (connection->flags & flags) != 0;
}
inline static int amqp_connection_is_running(const amqp_connection_t *connection)
{
    return amqp_connection_is(connection, AMQP_CONNECTION_RUNNING);
}

inline static void amqp_connection_flag_set(amqp_connection_t *connection, int flags)
{
    connection->flags |= flags;
}
inline static void amqp_connection_flag_clear(amqp_connection_t *connection, int flags)
{
    connection->flags &= ~flags;
}

inline static void amqp_connection_failure_flag_set(amqp_connection_t *connection, int flags)
{
    connection->failure_flags |= flags;
}
inline static void amqp_connection_failure_flag_clear(amqp_connection_t *connection, int flags)
{
    connection->failure_flags &= ~flags;
}

#ifdef __cplusplus
}
#endif
#endif
