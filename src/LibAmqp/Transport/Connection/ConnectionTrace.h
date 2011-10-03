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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_TRACE_H
#define LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_TRACE_H

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

typedef enum amqp_connection_trace_flag_t
{
    AMQP_TRACE_CONNECTION = 0X00000001,
    AMQP_TRACE_CONNECTION_SOCKET = 0X00000002,
    AMQP_TRACE_CONNECTION_READER = 0X00000004,
    AMQP_TRACE_CONNECTION_WRITER = 0X00000008,
    AMQP_TRACE_CONNECTION_NEGOTIATION = 0X00000010,
    AMQP_TRACE_CONNECTION_TLS = 0X00000020,
    AMQP_TRACE_CONNECTION_SASL = 0X00000040,
    AMQP_TRACE_CONNECTION_AMQP = 0X00000080,
    AMQP_TRACE_DISCONNECTS = 0X00000100,
    AMQP_TRACE_FRAME_READER = 0X00000200,
} amqp_connection_trace_flag_t;

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define amqp_connection_trace_transition(connection, flag, old_state_name, state_name) \
    if (connection->trace_flags & flag) \
    { \
        _amqp_connection_trace_transition(connection, __FILE__, __LINE__, flag, old_state_name, state_name); \
    }
#define amqp_connection_trace(connection, ...) _amqp_connection_trace(connection, __FILE__, __LINE__, "" __VA_ARGS__)
#else
#define amqp_connection_trace_transition(connection, flag, old_state_name, state_name)
#define amqp_connection_trace(connection, ...)
#endif

extern void _amqp_connection_trace(amqp_connection_t *connection, const char * filename, int line_number, const char *format, ...);
extern void _amqp_connection_trace_transition(amqp_connection_t *connection, const char * filename, int line_number, amqp_connection_trace_flag_t flag, const char *old_state_name, const char *state_name);

#define call_action(action, context, ...) \
if (action) { \
    action(__VA_ARGS__); \
} \
else { \
    amqp_error(context, AMQP_ERROR_UNITIALIZED_ACTION, "Calling " #action " before it was initialized."); \
}

#define amqp_connection_error(connection, code, ...) _amqp_connection_error(connection, __FILE__, __LINE__, #code, code, "" __VA_ARGS__)
extern void _amqp_connection_error(amqp_connection_t *connection, const char * filename, int line_number, const char *error_mnemonic, int error_code, const char *format, ...);

#define amqp_connection_failed(connection, code, flags, ...) _amqp_connection_failed(connection, __FILE__, __LINE__, #code, code, flags, "" __VA_ARGS__)
extern void _amqp_connection_failed(amqp_connection_t *connection, const char *filename, int line_number, const char *error_mnemonic, int error_code, uint32_t failure_flag, const char *format, ...);

#ifdef __cplusplus
}
#endif
#endif
