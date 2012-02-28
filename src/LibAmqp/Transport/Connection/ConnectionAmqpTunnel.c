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

DECLARE_TRANSITION_FUNCTION(initialized);
DECLARE_TRANSITION_FUNCTION(failed);
DECLARE_TRANSITION_FUNCTION(sending_header_response);
DECLARE_TRANSITION_FUNCTION(negotiated);

void amqp_connection_amqp_tunnel_initialize(amqp_connection_t *connection)
{
    transition_to_state(connection, initialized);
}

static void cleanup_resources(amqp_connection_t *connection)
{
}
void amqp_connection_amqp_tunnel_cleanup(amqp_connection_t *connection)
{
//    trace_cleanup();
    cleanup_resources(connection);
}

int amqp_connection_amqp_tunnel_is_state(const amqp_connection_t *connection, const char *state_name)
{
    return connection->state.amqp_tunnel.name != 0 ? (strcmp(connection->state.amqp_tunnel.name, state_name) == 0) : false;
}

static void amqp_done_callback(amqp_connection_t *connection)
{
    connection->state.amqp_tunnel.done(connection);
}

static void amqp_version_accepted(amqp_connection_t *connection)
{
    transition_to_state(connection, negotiated);
    connection->specification_version.supported.amqp = connection->specification_version.required.amqp;
    connection->state.connection.done(connection);
}
static void amqp_version_rejected(amqp_connection_t *connection, uint32_t version)
{
    transition_to_state(connection, failed);
    connection->specification_version.supported.amqp = version;
    connection->state.connection.fail(connection);
}
static void amqp_connect_while_initialized(amqp_connection_t *connection)
{
    connection->state.negotiator.reset(connection);
    connection->state.negotiator.start(connection, connection->specification_version.required.amqp, amqp_version_accepted, amqp_version_rejected);
}

static void tunnel_accept_while_initialized(amqp_connection_t *connection, uint32_t requested_version)
{
    uint32_t supported_version;

    assert(amqp_version_protocol_id(requested_version) == AMQP_PROTOCOL_ID);

    supported_version = amqp_negotiator_choose_amqp_protocol_version(connection, requested_version);

    if (requested_version != supported_version)
    {
        transition_to_state(connection, failed);
        amqp_connection_trace(connection, "requested amqp version: %08x, supported version: %08x", requested_version, supported_version);
        call_action(connection->state.connection.mode.server.reject, connection->context, connection, supported_version);
        return;
    }

    transition_to_state(connection, sending_header_response);
    call_action(connection->state.negotiator.send, connection->context, connection, supported_version, amqp_done_callback);
}
DEFINE_TRANSITION_TO_STATE(initialized)
{
    connection->state.amqp_tunnel.connect = amqp_connect_while_initialized;
    connection->state.amqp_tunnel.tunnel.accept = tunnel_accept_while_initialized;
}
DEFINE_TRANSITION_TO_STATE(failed)
{
}

static void done_while_sending_header_response(amqp_connection_t *connection)
{
    transition_to_state(connection, negotiated);
    call_action(connection->state.connection.done, connection->context, connection);
}
DEFINE_TRANSITION_TO_STATE(sending_header_response)
{
    connection->state.amqp_tunnel.done = done_while_sending_header_response;
}

DEFINE_TRANSITION_TO_STATE(negotiated)
{
}

/**********************************************
 Default states
 *********************************************/
static void illegal_state(amqp_connection_t *connection, const char *event)
{
    amqp_connection_error(connection, AMQP_ERROR_ILLEGAL_STATE,
            "Connection amqp tunnel does not support \"%s\" while \"%s\" and connection is \"%s\".",
            event, connection->state.amqp_tunnel.name, connection->state.connection.name);
    amqp_fatal_program_error("Connection amqp tunnel state error");
}

static void default_connect(amqp_connection_t *connection)
{
    illegal_state(connection, "Start");
}

static void default_done(amqp_connection_t *connection)
{
    illegal_state(connection, "Done");
}

static void default_tunnel_establish(amqp_connection_t *connection, uint32_t version)
{
    illegal_state(connection, "TunnelEstablish");
}

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection) LIBAMQP_TRACE_STATE_ARGS_DECL)
{
    save_old_state(connection->state.amqp_tunnel.name);

    connection->state.amqp_tunnel.connect = default_connect;
    connection->state.amqp_tunnel.done = default_done;
    connection->state.amqp_tunnel.tunnel.accept = default_tunnel_establish;

    connection->state.amqp_tunnel.name = state_name;

    action_initializer(connection);

    trace_state_transition(AMQP_TRACE_CONNECTION_AMQP_TUNNEL, connection->state.amqp_tunnel.name);
}
