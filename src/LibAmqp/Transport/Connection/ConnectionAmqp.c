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
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/ConnectionTrace.h"

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define save_old_state()  const char* old_state_name = connection->state.amqp.name
#define trace_transition(old_state_name) amqp_connection_trace_transition(connection, AMQP_TRACE_CONNECTION_AMQP, old_state_name, connection->state.amqp.name)
#else
#define save_old_state()
#define trace_transition(old_state_name)
#endif

static void transition_to_initialized(amqp_connection_t *connection);
static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name);
static void transition_to_failed(amqp_connection_t *connection);
static void transition_to_sending_header_response(amqp_connection_t *connection);
static void transition_to_negotiated(amqp_connection_t *connection);

void amqp_connection_amqp_initialize(amqp_connection_t *connection)
{
    transition_to_initialized(connection);
}

static void cleanup_resources(amqp_connection_t *connection)
{
}
void amqp_connection_amqp_cleanup(amqp_connection_t *connection)
{
//    trace_cleanup();
    cleanup_resources(connection);
}

int amqp_connection_amqp_is_state(const amqp_connection_t *connection, const char *state_name)
{
    return connection->state.amqp.name != 0 ? (strcmp(connection->state.amqp.name, state_name) == 0) : false;
}

static void amqp_done_callback(amqp_connection_t *connection)
{
    connection->state.amqp.done(connection);
}

static void amqp_version_accepted(amqp_connection_t *connection)
{
    transition_to_negotiated(connection);
    connection->specification_version.supported.amqp = connection->specification_version.required.amqp;
    connection->state.connection.done(connection);
}
static void amqp_version_rejected(amqp_connection_t *connection, uint32_t version)
{
    transition_to_failed(connection);
    connection->specification_version.supported.amqp = version;
    connection->state.connection.fail(connection);
}
static void amqp_connect_while_initialized(amqp_connection_t *connection)
{
    connection->state.negotiator.reset(connection);
    connection->state.negotiator.start(connection, connection->specification_version.required.amqp, amqp_version_accepted, amqp_version_rejected);
}

static void tunnel_establish_while_initialized(amqp_connection_t *connection, uint32_t requested_version)
{
    uint32_t supported_version;

    assert(amqp_version_protocol_id(requested_version) == AMQP_PROTOCOL_ID);

    supported_version = amqp_negotiator_choose_amqp_protocol_version(connection, requested_version);

    if (requested_version != supported_version)
    {
        transition_to_failed(connection);
        amqp_connection_trace(connection, "requested amqp version: %08x, supported version: %08x", requested_version, supported_version);
        call_action(connection->state.connection.mode.server.reject, connection->context, connection, supported_version);
        return;
    }

    transition_to_sending_header_response(connection);
    call_action(connection->state.negotiator.send, connection->context, connection, supported_version, amqp_done_callback);
}
static void transition_to_initialized(amqp_connection_t *connection)
{
    default_state_initialization(connection, "Initialized");
    connection->state.amqp.connect = amqp_connect_while_initialized;
    connection->state.amqp.tunnel.accept = tunnel_establish_while_initialized;
    trace_transition("Created");
}
static void transition_to_failed(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "Failed");
    trace_transition(old_state_name);
}

static void done_while_sending_header_response(amqp_connection_t *connection)
{
    transition_to_negotiated(connection);
    call_action(connection->state.connection.done, connection->context, connection);
}
static void transition_to_sending_header_response(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "SendingHeader");
    connection->state.amqp.done = done_while_sending_header_response;
    trace_transition(old_state_name);
}

static void transition_to_negotiated(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "Negotiated");
    trace_transition(old_state_name);
}

/**********************************************
 Default states
 *********************************************/
static void illegal_state(amqp_connection_t *connection, const char *event)
{
    amqp_connection_error(connection, AMQP_ERROR_ILLEGAL_STATE,
            "Connection amqp tunnel does not support \"%s\" while \"%s\" and connection is \"%s\".",
            event, connection->state.amqp.name, connection->state.connection.name);
    amqp_fatal_program_error("Connection amqp state error");
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

static void default_confirm(amqp_connection_t *connection)
{
    illegal_state(connection, "VersionConfirm");
}

static void default_reject(amqp_connection_t *connection, uint32_t version)
{
    illegal_state(connection, "VersionReject");
}

static void default_request(amqp_connection_t *connection, uint32_t version)
{
    illegal_state(connection, "ClientRequest");
}

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name)
{
    connection->state.amqp.connect = default_connect;
    connection->state.amqp.done = default_done;

    connection->state.amqp.tunnel.accept = default_tunnel_establish;
    connection->state.amqp.negotiation.confirm = default_confirm;
    connection->state.amqp.negotiation.reject = default_reject;
    connection->state.amqp.negotiation.request = default_request;
    connection->state.amqp.name = new_state_name;
}
