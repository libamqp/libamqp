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

#include <stdarg.h>
#include <errno.h>
#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#include "Context/Context.h"
#include "Misc/Bits.h"
#include "Transport/Connection/ConnectionTrace.h"
#include "Transport/Connection/Connection.h"

#include "debug_helper.h"

const char *labels[] = {
    "connection", "socket", "reader", "writer", "negotiation", "tls", "sasl", "amqp"
};

static void connection_source(amqp_connection_t *connection, char *buffer, size_t buffer_size)
{
    snprintf(buffer, buffer_size, "%s:%d", connection->socket.hostname ? connection->socket.hostname : "no-peer", connection->socket.port_number);
}

void _vamqp_connection_error(amqp_connection_t *connection, const char *filename, int line_number, const char *error_mnemonic, int error_code, const char *format, va_list args)
{
//    va_list args;
    char source[128];

    connection_source(connection, source, sizeof(source));

//    va_start(args, format);
    _vamqp_error(connection->context, 1, filename, line_number, source, error_mnemonic, error_code, format, args);
//    va_end(args);

    _amqp_debug(connection->context, 2, filename, line_number, source, 0,
            "connection state - %s:socket=%s:writer=%s:reader=%s:tls=%s:sasl=%s:amqp=%s:neg=%s",
            connection->state.connection.name,
            connection->state.socket.name,
            connection->state.writer.name,
            connection->state.reader.name,
            connection->state.tls.name,
            connection->state.sasl.name,
            connection->state.amqp.name,
            connection->state.negotiator.name);
}

void _amqp_connection_error(amqp_connection_t *connection, const char *filename, int line_number, const char *error_mnemonic, int error_code, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    _vamqp_connection_error(connection, filename, line_number, error_mnemonic, error_code, format, args);
    va_end(args);
}


void _amqp_connection_failed(amqp_connection_t *connection, const char *filename, int line_number, const char *error_mnemonic, int error_code, uint32_t failure_flag, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    _vamqp_connection_error(connection, filename, line_number, error_mnemonic, error_code, format, args);
    va_end(args);

    amqp_connection_failure_flag_set(connection, failure_flag);
    connection->state.connection.fail(connection);
}

void _amqp_connection_trace(amqp_connection_t *connection, const char * filename, int line_number, const char *format, ...)
{
    va_list args;
    char source[128];

    connection_source(connection, source, sizeof(source));

    va_start(args, format);
    _vamqp_trace(connection->context, 2, filename, line_number, source, format, args);
    va_end(args);
}

void _amqp_connection_trace_transition(amqp_connection_t *connection, const char * filename, int line_number, amqp_connection_trace_flag_t flag, const char *old_state_name, const char *state_name)
{
    const int size = sizeof(labels) / sizeof(char *);

    int index = amqp_trailing_zeros_32(flag);
    const char *label = index < size ? labels[index] : "Doh!";

    _amqp_connection_trace(connection, filename, line_number, "%s - transitioned from %s to %s", label, old_state_name, state_name);
}

