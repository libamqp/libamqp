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
#include "Transport/Connection/ConnectionTestHook.h"

#include "debug_helper.h"

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define save_old_state()  const char* old_state_name = connection->state.connection.name
#define trace_transition(old_state_name) amqp_connection_trace_transition(connection, AMQP_TRACE_CONNECTION, old_state_name, connection->state.connection.name)
#else
#define save_old_state()
#define trace_transition(old_state_name)
#endif

static void transition_to_waiting_on_client_negotiation_request(amqp_connection_t *connection);
static void transition_to_accepting_tls(amqp_connection_t *connection);
static void transition_to_waiting_on_tls_tunnelled_request(amqp_connection_t *connection);
static void transition_to_accepting_sasl(amqp_connection_t *connection);
static void transition_to_waiting_on_amqp_negotiation_request(amqp_connection_t *connection);
static void transition_to_accepting_amqp(amqp_connection_t *connection);
static void transition_to_tunnel_accepted(amqp_connection_t *connection);
static void transition_to_sending_error_message(amqp_connection_t *connection);

static void client_request_callback(amqp_connection_t *connection, uint32_t requested_protocol_version)
{
    connection->state.connection.mode.server.begin(connection, requested_protocol_version);
}
static void connection_done_callback(amqp_connection_t *connection)
{
    connection->state.connection.done(connection);
}

void amqp_connection_server_state_initialize(amqp_connection_t *connection, int fd, struct sockaddr_storage *client_address, socklen_t address_size)
{
    transition_to_waiting_on_client_negotiation_request(connection);

    amqp_connection_state_initialize(connection);
    amqp_connection_accepted_socket_initialize(connection, fd, client_address, address_size);
    amqp_connection_enable_io(connection);

    amqp_call_test_hook(connection, post_initialize);

    connection->state.negotiator.wait(connection, client_request_callback);
}

static void reject_protocol_version(amqp_connection_t *connection, uint32_t version)
{
    transition_to_sending_error_message(connection);
    connection->state.negotiator.send(connection, version, connection_done_callback);
}
static void could_not_parse_header(amqp_connection_t *connection)
{
    amqp_error(connection->context, AMQP_ERROR_INVALID_HEADER, "Cannot decode header");
    reject_protocol_version(connection, AMQP_PREFERRED_VERSION);
}
static void unsupported_protocol(amqp_connection_t *connection, uint32_t version)
{
    amqp_error(connection->context, AMQP_ERROR_UNKNOWN_PROTOCOL, "Unsupported protocol id in AMQP header. ID: %d", amqp_version_protocol_id(version));
    reject_protocol_version(connection, AMQP_PREFERRED_VERSION);
}

static int is_protocol_valid_now(int protocol_id, int accepted_protocols)
{
    if ((accepted_protocols & AMQP_PROTOCOL_SASL) != 0 && protocol_id == AMQP_SASL_PROTOCOL_ID)
    {
        return true;
    }
    if ((accepted_protocols & AMQP_PROTOCOL_AMQP) != 0 && protocol_id == AMQP_PROTOCOL_ID)
    {
        return true;
    }
    if ((accepted_protocols & AMQP_PROTOCOL_TLS) != 0 && protocol_id == AMQP_TLS_PROTOCOL_ID)
    {
        return true;
    }
    return false;
}

static void establish_tunnel_after_client_request(amqp_connection_t *connection, uint32_t requested_protocol_version, int accepted_protocols)
{
    int protocol_id;
    if (requested_protocol_version == 0)
    {
        could_not_parse_header(connection);
        return;
    }

    protocol_id = amqp_version_protocol_id(requested_protocol_version);
    if (!is_protocol_valid_now(protocol_id, accepted_protocols))
    {
        unsupported_protocol(connection, requested_protocol_version);
        return;
    }

    switch (protocol_id)
    {
    case AMQP_TLS_PROTOCOL_ID:
        transition_to_accepting_tls(connection);
        call_action(connection->state.tls.tunnel.accept, connection->context, connection, requested_protocol_version);
        break;

    case AMQP_SASL_PROTOCOL_ID:
        transition_to_accepting_sasl(connection);
        call_action(connection->state.sasl.tunnel.accept, connection->context, connection, requested_protocol_version);
        break;

    case AMQP_PROTOCOL_ID:
        transition_to_accepting_amqp(connection);
        call_action(connection->state.amqp_tunnel.tunnel.accept, connection->context, connection, requested_protocol_version);
        break;

    default:
        illegal_program_state(unknown protocol id)
        break;
    }
}

static void begin_while_waiting_on_client_negotiation_request(amqp_connection_t *connection, uint32_t requested_protocol_version)
{
    int accepted_protocols = AMQP_PROTOCOL_AMQP | AMQP_PROTOCOL_SASL | AMQP_PROTOCOL_TLS;
    establish_tunnel_after_client_request(connection, requested_protocol_version, accepted_protocols);
}
static void transition_to_waiting_on_client_negotiation_request(amqp_connection_t *connection)
{
    save_old_state();

    amqp__connection_default_state_initialization(connection, "WaitingForClientNegotiation");
    connection->state.connection.mode.server.begin = begin_while_waiting_on_client_negotiation_request;

    trace_transition(old_state_name);
}

static void done_while_sending_error_message(amqp_connection_t *connection)
{
    connection->state.connection.drain(connection);
}
static void transition_to_sending_error_message(amqp_connection_t *connection)
{
    save_old_state();
    amqp__connection_default_state_initialization(connection, "SendingErrorHeaderResponse");
    connection->state.connection.done = done_while_sending_error_message;
    trace_transition(old_state_name);
}

static void done_while_accepting_tls(amqp_connection_t *connection)
{
    transition_to_waiting_on_tls_tunnelled_request(connection);
    amqp_connection_flag_set(connection, AMQP_CONNECTION_TLS_CONNECTED);
    connection->state.negotiator.reset(connection);
    connection->state.negotiator.wait(connection, client_request_callback);
    not_implemented(done_while_accepting_tls);
}
static void fail_while_accepting_tls(amqp_connection_t *connection)
{
    not_implemented(fail_while_accepting_tls);
}
static void reject_while_accepting_tls(amqp_connection_t *connection, uint32_t version)
{
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_TLS_NEGOTIATION_REJECTED);
    reject_protocol_version(connection, version);
}
static void transition_to_accepting_tls(amqp_connection_t *connection)
{
    save_old_state();

    amqp__connection_default_state_initialization(connection, "AcceptingTls");
    connection->state.connection.done = done_while_accepting_tls;
    connection->state.connection.fail = fail_while_accepting_tls;
    connection->state.connection.mode.server.reject = reject_while_accepting_tls;

    trace_transition(old_state_name);
}

static void begin_while_waiting_on_tls_tunnelled_request(amqp_connection_t *connection, uint32_t requested_protocol_version)
{
    int accepted_protocols = AMQP_PROTOCOL_AMQP | AMQP_PROTOCOL_SASL;
    establish_tunnel_after_client_request(connection, requested_protocol_version, accepted_protocols);
}
static void transition_to_waiting_on_tls_tunnelled_request(amqp_connection_t *connection)
{
    save_old_state();
    amqp__connection_default_state_initialization(connection, "WaitingForTlsTunnelled");
    connection->state.connection.mode.server.begin = begin_while_waiting_on_tls_tunnelled_request;
    trace_transition(old_state_name);
}

static void done_while_accepting_sasl(amqp_connection_t *connection)
{
    transition_to_waiting_on_amqp_negotiation_request(connection);
    amqp_connection_flag_set(connection, AMQP_CONNECTION_SASL_CONNECTED);
    connection->state.negotiator.reset(connection);
    connection->state.negotiator.wait(connection, client_request_callback);
}
static void fail_while_accepting_sasl(amqp_connection_t *connection)
{
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_SASL_NEGOTIATION_REJECTED);
//    amqp_connection_trace(connection, "SASL negotiation failed");
    connection->state.connection.drain(connection);
}
static void reject_while_accepting_sasl(amqp_connection_t *connection, uint32_t version)
{
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_SASL_NEGOTIATION_REJECTED);
    reject_protocol_version(connection, version);
}
static void transition_to_accepting_sasl(amqp_connection_t *connection)
{
    save_old_state();

    amqp__connection_default_state_initialization(connection, "AcceptingSasl");
    connection->state.connection.done = done_while_accepting_sasl;
    connection->state.connection.fail = fail_while_accepting_sasl;
    connection->state.connection.mode.server.reject = reject_while_accepting_sasl;

    trace_transition(old_state_name);
}

static void begin_while_waiting_on_amqp_negotiation_request(amqp_connection_t *connection, uint32_t requested_protocol_version)
{
    int accepted_protocols = AMQP_PROTOCOL_AMQP;
    establish_tunnel_after_client_request(connection, requested_protocol_version, accepted_protocols);
}
static void transition_to_waiting_on_amqp_negotiation_request(amqp_connection_t *connection)
{
    save_old_state();

    amqp__connection_default_state_initialization(connection, "WaitingForAmqpRequest");
    connection->state.connection.mode.server.begin = begin_while_waiting_on_amqp_negotiation_request;

    trace_transition(old_state_name);
}

static void done_while_accepting_amqp(amqp_connection_t *connection)
{
    amqp_connection_flag_set(connection, AMQP_CONNECTION_AMQP_CONNECTED);
    transition_to_tunnel_accepted(connection);
    // TODO - wait for the client to establish a link
}
static void fail_while_accepting_amqp(amqp_connection_t *connection)
{
    not_implemented(fail_while_accepting_amqp);
}
static void reject_while_accepting_amqp(amqp_connection_t *connection, uint32_t version)
{
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_AMQP_NEGOTIATION_REJECTED);
    reject_protocol_version(connection, version);
}
static void transition_to_accepting_amqp(amqp_connection_t *connection)
{
    save_old_state();

    amqp__connection_default_state_initialization(connection, "AcceptingAmqp");
    connection->state.connection.done = done_while_accepting_amqp;
    connection->state.connection.fail = fail_while_accepting_amqp;
    connection->state.connection.mode.server.reject = reject_while_accepting_amqp;

    trace_transition(old_state_name);
}

static void shutdown_while_tunnel_accepted(amqp_connection_t *connection)
{
    connection->state.amqp.close(connection);
}
static void transition_to_tunnel_accepted(amqp_connection_t *connection)
{
    save_old_state();
    amqp__connection_default_state_initialization(connection, "AmqpTunnelAccepted");
    connection->state.connection.shutdown = shutdown_while_tunnel_accepted;
    trace_transition(old_state_name);
}
