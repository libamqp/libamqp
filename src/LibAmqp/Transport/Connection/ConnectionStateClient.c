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

#include "Context/Context.h"
#include "Transport/Connection/Connections.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/ConnectionTrace.h"
#include "debug_helper.h"

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define save_old_state()  const char* old_state_name = connection->state.connection.name
#define trace_transition(old_state_name) amqp_connection_trace_transition(connection, AMQP_TRACE_CONNECTION, old_state_name, connection->state.connection.name)
#else
#define save_old_state()
#define trace_transition(old_state_name)
#endif

static void transition_to_initialized(amqp_connection_t *connection);
static void transition_to_connecting_socket(amqp_connection_t *connection);
static void transition_to_connecting_tls(amqp_connection_t *connection);
static void transition_to_connecting_sasl(amqp_connection_t *connection);
static void transition_to_connecting_amqp(amqp_connection_t *connection);
static void transition_to_amqp_tunnel_established(amqp_connection_t *connection);

void amqp_connection_client_state_initialize(amqp_connection_t *connection)
{
    transition_to_initialized(connection);
    amqp_connection_socket_initialize(connection);
    amqp_connection_state_initialize(connection);
}

static void connect_while_initialized(amqp_connection_t *connection, const char *hostname, int port_number)
{
    transition_to_connecting_socket(connection);
    amqp_connection_flag_set(connection, AMQP_CONNECTION_RUNNING);
    connection->state.socket.connect(connection, hostname, port_number);
}
static void transition_to_initialized(amqp_connection_t *connection)
{
    amqp__connection_default_state_initialization(connection, "Initialized");
    connection->state.connection.mode.client.connect = connect_while_initialized;
    trace_transition("Created");
}

static void clear_protocol_flag(amqp_connection_t *connection, int flag)
{
    connection->data.handshake.protocols &= ~flag;
}
static void start_next_protocol_on_connection(amqp_connection_t *connection)
{
    if (connection->data.handshake.protocols & AMQP_PROTOCOL_TLS)
    {
        transition_to_connecting_tls(connection);
        clear_protocol_flag(connection, AMQP_PROTOCOL_TLS);
        connection->state.tls.connect(connection);
        return;
    }
    if (connection->data.handshake.protocols & AMQP_PROTOCOL_SASL)
    {
        transition_to_connecting_sasl(connection);
        clear_protocol_flag(connection, AMQP_PROTOCOL_SASL);
        connection->state.sasl.connect(connection);
        return;
    }
    if (connection->data.handshake.protocols & AMQP_PROTOCOL_AMQP)
    {
        transition_to_connecting_amqp(connection);
        clear_protocol_flag(connection, AMQP_PROTOCOL_AMQP);
        connection->state.amqp_tunnel.connect(connection);
        return;
    }

    amqp_error(connection->context, AMQP_ERROR_NO_PROTOCOL, "No Protocol requested or requested protocols were not supported.");
    connection->state.connection.fail(connection);
}

static void done_while_connecting_socket(amqp_connection_t *connection)
{
//printf("Connected to %s:%d\n", connection->socket.hostname, connection->socket.port_number);
//fflush(stdout);

    amqp_connection_flag_set(connection, AMQP_CONNECTION_SOCKET_CONNECTED);
    amqp_connection_enable_io(connection);
    connection->data.handshake.protocols = connection->protocols;

    start_next_protocol_on_connection(connection);
}
static void fail_while_connecting_socket(amqp_connection_t *connection)
{
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_SOCKET_CONNECT_FAILED);
    not_implemented(fail_while_connecting_tls);
}
static void transition_to_connecting_socket(amqp_connection_t *connection)
{
    save_old_state();

    amqp__connection_default_state_initialization(connection, "ConnectingSocket");
    connection->state.connection.done = done_while_connecting_socket;
    connection->state.connection.fail = fail_while_connecting_socket;

    trace_transition(old_state_name);
}

static void done_while_connecting_tls(amqp_connection_t *connection)
{
    amqp_connection_flag_set(connection, AMQP_CONNECTION_TLS_CONNECTED);
    start_next_protocol_on_connection(connection);
}
static void fail_while_connecting_tls(amqp_connection_t *connection)
{
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_TLS_NEGOTIATION_REJECTED);
    not_implemented(fail_while_connecting_tls);
}
static void transition_to_connecting_tls(amqp_connection_t *connection)
{
    save_old_state();

    amqp__connection_default_state_initialization(connection, "ConnectingTls");
    connection->state.connection.done = done_while_connecting_tls;
    connection->state.connection.fail = fail_while_connecting_tls;

    trace_transition(old_state_name);
}

static void done_while_connecting_sasl(amqp_connection_t *connection)
{
    amqp_connection_flag_set(connection, AMQP_CONNECTION_SASL_CONNECTED);
    start_next_protocol_on_connection(connection);
}
static void fail_while_connecting_sasl(amqp_connection_t *connection)
{
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_SASL_NEGOTIATION_REJECTED);
    amqp_connection_trace(connection, "SASL negotiation failed");
    connection->state.shutdown.drain(connection);
}
static void transition_to_connecting_sasl(amqp_connection_t *connection)
{
    save_old_state();

    amqp__connection_default_state_initialization(connection, "ConnectingSasl");
    connection->state.connection.done = done_while_connecting_sasl;
    connection->state.connection.fail = fail_while_connecting_sasl;

    trace_transition(old_state_name);
}

static void done_while_connecting_amqp(amqp_connection_t *connection)
{
    amqp_connection_flag_set(connection, AMQP_CONNECTION_AMQP_CONNECTED | AMQP_CONNECTION_IS_CLIENT);
    transition_to_amqp_tunnel_established(connection);
    connection->state.amqp.send_open(connection);
}
static void fail_while_connecting_amqp(amqp_connection_t *connection)
{
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_AMQP_NEGOTIATION_REJECTED);
    amqp_connection_trace(connection, "AMQP negotiation failed");
    connection->state.shutdown.drain(connection);
}
static void transition_to_connecting_amqp(amqp_connection_t *connection)
{
    save_old_state();

    amqp__connection_default_state_initialization(connection, "ConnectingAmqp");
    connection->state.connection.done = done_while_connecting_amqp;
    connection->state.connection.fail = fail_while_connecting_amqp;

    trace_transition(old_state_name);
}

static void transition_to_amqp_tunnel_established(amqp_connection_t *connection)
{
    save_old_state();
    amqp__connection_default_state_initialization(connection, "AqmpTunnelEstablished");
    trace_transition(old_state_name);
}
