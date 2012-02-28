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

#include "Transport/Connection/ConnectionStateMachine.h"

#include "Transport/Connection/AcceptedConnections.h"
#include "Transport/Connection/ConnectionTestHook.h"

DECLARE_TRANSITION_FUNCTION(waiting_on_client_negotiation_request);
DECLARE_TRANSITION_FUNCTION(accepting_tls);
DECLARE_TRANSITION_FUNCTION(waiting_on_tls_tunnelled_request);
DECLARE_TRANSITION_FUNCTION(accepting_sasl);
DECLARE_TRANSITION_FUNCTION(waiting_on_amqp_negotiation_request);
DECLARE_TRANSITION_FUNCTION(accepting_amqp);
DECLARE_TRANSITION_FUNCTION(tunnel_accepted);
DECLARE_TRANSITION_FUNCTION(sending_error_message);

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
    transition_to_state(connection, waiting_on_client_negotiation_request);

    amqp_connection_state_initialize(connection);
    amqp_connection_accepted_socket_initialize(connection, fd, client_address, address_size);
    amqp_connection_enable_io(connection);

    amqp_call_test_hook(connection, post_initialize);

    connection->state.negotiator.wait(connection, client_request_callback);
}

static void reject_protocol_version(amqp_connection_t *connection, uint32_t version)
{
    transition_to_state(connection, sending_error_message);
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
        transition_to_state(connection, accepting_tls);
        call_action(connection->state.tls.tunnel.accept, connection->context, connection, requested_protocol_version);
        break;

    case AMQP_SASL_PROTOCOL_ID:
        transition_to_state(connection, accepting_sasl);
        call_action(connection->state.sasl.tunnel.accept, connection->context, connection, requested_protocol_version);
        break;

    case AMQP_PROTOCOL_ID:
        transition_to_state(connection, accepting_amqp);
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
DEFINE_TRANSITION_TO_STATE(waiting_on_client_negotiation_request)
{
    connection->state.connection.mode.server.begin = begin_while_waiting_on_client_negotiation_request;
}

static void done_while_sending_error_message(amqp_connection_t *connection)
{
    connection->state.shutdown.drain(connection);
}
DEFINE_TRANSITION_TO_STATE(sending_error_message)
{
    connection->state.connection.done = done_while_sending_error_message;
}

static void done_while_accepting_tls(amqp_connection_t *connection)
{
    transition_to_state(connection, waiting_on_tls_tunnelled_request);
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
DEFINE_TRANSITION_TO_STATE(accepting_tls)
{
    connection->state.connection.done = done_while_accepting_tls;
    connection->state.connection.fail = fail_while_accepting_tls;
    connection->state.connection.mode.server.reject = reject_while_accepting_tls;
}

static void begin_while_waiting_on_tls_tunnelled_request(amqp_connection_t *connection, uint32_t requested_protocol_version)
{
    int accepted_protocols = AMQP_PROTOCOL_AMQP | AMQP_PROTOCOL_SASL;
    establish_tunnel_after_client_request(connection, requested_protocol_version, accepted_protocols);
}
DEFINE_TRANSITION_TO_STATE(waiting_on_tls_tunnelled_request)
{
    connection->state.connection.mode.server.begin = begin_while_waiting_on_tls_tunnelled_request;
}

static void done_while_accepting_sasl(amqp_connection_t *connection)
{
    transition_to_state(connection, waiting_on_amqp_negotiation_request);
    amqp_connection_flag_set(connection, AMQP_CONNECTION_SASL_CONNECTED);
    connection->state.negotiator.reset(connection);
    connection->state.negotiator.wait(connection, client_request_callback);
}
static void fail_while_accepting_sasl(amqp_connection_t *connection)
{
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_SASL_NEGOTIATION_REJECTED);
//    amqp_connection_trace(connection, "SASL negotiation failed");
    connection->state.shutdown.drain(connection);
}
static void reject_while_accepting_sasl(amqp_connection_t *connection, uint32_t version)
{
    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_SASL_NEGOTIATION_REJECTED);
    reject_protocol_version(connection, version);
}
DEFINE_TRANSITION_TO_STATE(accepting_sasl)
{
    connection->state.connection.done = done_while_accepting_sasl;
    connection->state.connection.fail = fail_while_accepting_sasl;
    connection->state.connection.mode.server.reject = reject_while_accepting_sasl;
}

static void begin_while_waiting_on_amqp_negotiation_request(amqp_connection_t *connection, uint32_t requested_protocol_version)
{
    int accepted_protocols = AMQP_PROTOCOL_AMQP;
    establish_tunnel_after_client_request(connection, requested_protocol_version, accepted_protocols);
}
DEFINE_TRANSITION_TO_STATE(waiting_on_amqp_negotiation_request)
{
    connection->state.connection.mode.server.begin = begin_while_waiting_on_amqp_negotiation_request;
}

static void done_while_accepting_amqp(amqp_connection_t *connection)
{
    amqp_connection_flag_set(connection, AMQP_CONNECTION_AMQP_CONNECTED);
    transition_to_state(connection, tunnel_accepted);
    connection->state.amqp.wait_on_open(connection);
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
DEFINE_TRANSITION_TO_STATE(accepting_amqp)
{
    connection->state.connection.done = done_while_accepting_amqp;
    connection->state.connection.fail = fail_while_accepting_amqp;
    connection->state.connection.mode.server.reject = reject_while_accepting_amqp;
}

DEFINE_TRANSITION_TO_STATE(tunnel_accepted)
{
}

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection) LIBAMQP_TRACE_STATE_ARGS_DECL)
{
    save_old_state(connection->state.connection.name);

    amqp__connection_default_state_initialization(connection, state_name);

    action_initializer(connection);

    trace_state_transition(AMQP_TRACE_CONNECTION, connection->state.connection.name);
}
