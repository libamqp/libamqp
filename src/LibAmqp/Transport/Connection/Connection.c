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

#include "Context/Context.h"
#include "Transport/LowLevel/ListenerThread.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/AcceptedConnections.h"

#include "Transport/Connection/ConnectionTrace.h"

static amqp_connection_t *create_connection(amqp_context_t *context)
{
    amqp_connection_t *result = AMQP_MALLOC(context, amqp_connection_t);
    result->context = context;
    result->protocols = AMQP_PROTOCOL_SASL | AMQP_PROTOCOL_AMQP;

    result->specification_version.required.tls = AMQP_TLS_PREFERRED_VERSION;
    result->specification_version.required.sasl = AMQP_SASL_PREFERRED_VERSION;
    result->specification_version.required.amqp = AMQP_PREFERRED_VERSION;

    result->amqp.connection.limits.max_frame_size = context->limits.max_frame_size;
    result->amqp.connection.limits.channel_max = context->limits.channel_max;
    result->amqp.connection.limits.idle_time_out = context->limits.idle_time_out;

    if (result->amqp.connection.limits.max_frame_size < AMQP_MIN_MAX_FRAME_SIZE)
    {
        result->amqp.connection.limits.max_frame_size = AMQP_MIN_MAX_FRAME_SIZE;
    }

    assert(context->reference.container_id);
    result->amqp.connection.local_container_id = amqp_duplicate_cstr(context, context->reference.container_id);

    result->sasl.identity_hooks = context->sasl.identity_hooks;
    return result;
}

amqp_connection_t *amqp_connection_create(amqp_context_t *context)
{
    amqp_connection_t *result = create_connection(context);

    amqp_connection_client_state_initialize(result);
    return result;
}

void amqp_connection_destroy(amqp_context_t *context, amqp_connection_t *connection)
{
    connection->state.connection.shutdown(connection);
    amqp_connection_state_cleanup(connection);
    AMQP_FREE(context, connection);
}

void amqp_connection_connect(amqp_context_t *context, amqp_connection_t *connection)
{
    assert(context && context->thread_event_loop);
    // Connect to the configured host:port
    not_implemented(amqp_connection_connect);
}

void amqp_connection_connect_to(amqp_context_t *context, amqp_connection_t *connection, const char *hostname, int port_number)
{
    assert(context && context->thread_event_loop);
    if ((connection->protocols & AMQP_PROTOCOL_AMQP) == 0)
    {
        amqp_connection_error(connection, AMQP_ERROR_NO_PROTOCOL, "AMQP protocol must be selected");
        connection->state.connection.fail(connection);
        return;
    }
    connection->state.connection.mode.client.connect(connection, hostname, port_number);
}

amqp_connection_t *amqp_connection_accept(amqp_context_t *context, int fd, struct sockaddr_storage *client_address, socklen_t address_size, amqp_accept_handler_arguments_t *arguments)
{
    amqp_connection_t *result;
    
    assert(arguments->connections != 0);
    assert(context && context->thread_event_loop);

    result = create_connection(context);
    result->state.connection.name = result->state.socket.name = "Initialized";
    result->accept_handler_arguments = arguments;

    amqp_connection_server_state_initialize(result, fd, client_address, address_size);
//    result->trace_flags  = ~(AMQP_TRACE_CONNECTION_READER | AMQP_TRACE_CONNECTION_WRITER | AMQP_TRACE_CONNECTION_NEGOTIATION);
    amqp_connections_add(context, arguments->connections, result);
    return result;
}

void amqp_connection_shutdown_confirm(amqp_context_t *context, amqp_connection_t *connection)
{
    assert(connection && connection->context && connection->context->thread_event_loop);
    if (connection->accept_handler_arguments)
    {
        amqp_connections_shutdown_confirm(connection->accept_handler_arguments->connections);
        amqp_connection_destroy(context, connection);
    }
}

void amqp_connection_shutdown(amqp_context_t *context, amqp_connection_t *connection)
{
    assert(connection);
    assert(connection->context);
    assert(connection->context->thread_event_loop);

    trace_application_command(connection, "amqp_connection_shutdown() called");
    connection->state.connection.shutdown(connection);
}

void amqp_connection_read(amqp_context_t *context, amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required)
{
    assert(connection && connection->context && connection->context->thread_event_loop);
    not_implemented(amqp_connection_read);
}

const char *amqp_connection_target_host(amqp_connection_t *connection)
{
    return connection->socket.hostname ? connection->socket.hostname : "unknown-host";
}
