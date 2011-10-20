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
#include "Transport/Frame/Frame.h"
#include "Transport/Sasl/SaslMechanisms.h"
#include "debug_helper.h"

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define save_old_state()  const char* old_state_name = connection->state.sasl.name
#define trace_transition(old_state_name) amqp_connection_trace_transition(connection, AMQP_TRACE_CONNECTION_SASL, old_state_name, connection->state.sasl.name)
#else
#define save_old_state()
#define trace_transition(old_state_name)
#endif

static void transition_to_initialized(amqp_connection_t *connection);
static void transition_to_waiting_on_sasl_mechanisms(amqp_connection_t *connection);
static void transition_to_failed(amqp_connection_t *connection);
static void transition_to_negotiated(amqp_connection_t *connection);
static void transition_to_sending_header_response(amqp_connection_t *connection);

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name);

void amqp_connection_sasl_initialize(amqp_connection_t *connection)
{
    transition_to_initialized(connection);
}

static void cleanup_resources(amqp_connection_t *connection)
{
}
void amqp_connection_sasl_cleanup(amqp_connection_t *connection)
{
    cleanup_resources(connection);
}

int amqp_connection_sasl_is_state(const amqp_connection_t *connection, const char *state_name)
{
    return connection->state.sasl.name != 0 ? (strcmp(connection->state.sasl.name, state_name) == 0) : false;
}

static
void frame_available_from_server_callback(amqp_connection_t *connection, amqp_buffer_t *buffer)
{
    amqp_frame_t *frame = amqp_decode_sasl_frame(connection->context, buffer);

    if (frame)
    {
        frame->dispatch(connection, frame);
    }
    else
    {
        amqp_connection_failed(connection, AMQP_ERROR_FRAME_DECODE_FAILED, AMQP_CONNECTION_SASL_FRAME_DECODE_ERROR, "Could not decode frame from broker");
    }
}

static void sasl_broker_version_sent_callback(amqp_connection_t *connection)
{
    connection->state.sasl.done(connection);
}

static void sasl_version_accepted_callback(amqp_connection_t *connection)
{
    // The broker has accepted our preferred sasl version and will next send it's supported SASL mechanisms
    amqp_connection_trace(connection, "SASL version accepted");
    connection->specification_version.supported.sasl = connection->specification_version.required.sasl;
    transition_to_waiting_on_sasl_mechanisms(connection);

    // TODO - no longer - connection->state.connection.done(connection);
    
    connection->state.frame.enable(connection);             // TODO - consider this redundant?
    amqp_buffer_reset(connection->buffer.read);
    connection->state.frame.read(connection, connection->buffer.read, frame_available_from_server_callback);
}
static void sasl_version_rejected_callback(amqp_connection_t *connection, uint32_t version)
{
    amqp_connection_trace(connection, "SASL version rejected");
    transition_to_failed(connection);
    connection->specification_version.supported.sasl = version;
    connection->state.connection.fail(connection);
}
static void sasl_connect_while_initialized(amqp_connection_t *connection)
{
    connection->state.negotiator.reset(connection);
    connection->state.negotiator.start(connection, connection->specification_version.required.sasl, sasl_version_accepted_callback, sasl_version_rejected_callback);
}
static void tunnel_accept_while_initialized(amqp_connection_t *connection, uint32_t requested_version)
{
    uint32_t supported_version;
    assert(amqp_version_protocol_id(requested_version) == AMQP_SASL_PROTOCOL_ID);

    supported_version = amqp_negotiator_choose_sasl_protocol_version(connection, requested_version);

    if (requested_version != supported_version)
    {
        transition_to_failed(connection);
        amqp_connection_trace(connection, "requested sasl version: %08x, supported version: %08x", requested_version, supported_version);
        call_action(connection->state.connection.mode.server.reject, connection->context, connection, supported_version);
        return;
    }

    transition_to_sending_header_response(connection);
    call_action(connection->state.negotiator.send, connection->context, connection, supported_version, sasl_broker_version_sent_callback);
}
static void transition_to_initialized(amqp_connection_t *connection)
{
    default_state_initialization(connection, "Initialized");
    connection->state.sasl.connect = sasl_connect_while_initialized;
    connection->state.sasl.tunnel.accept = tunnel_accept_while_initialized;
    trace_transition("Created");
}
static void sasl_mechanisms_while_waiting_on_sasl_mechanisms(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_multiple_symbol_t *multiple = &frame->frames.sasl.mechanisms.sasl_server_mechanisms;
    amqp_sasl_plugin_t *plugin = amqp_sasl_select_mechanism(connection, multiple);
    if (plugin)
    {
        not_implemented(todo);
    }
    else
    {
        int space = amqp_multiple_symbol_total_length(multiple) + (amqp_multiple_symbol_size(multiple) - 1) * 2 + 1;
        char *buffer = amqp_allocate_print_buffer(connection->context, space);
        amqp_multiple_symbol_to_char_bytes(multiple, ", ", buffer, space);
        amqp_connection_failed(connection, AMQP_ERROR_NO_SASL_MECHANISM, AMQP_CONNECTION_SASL_ERROR, "No SASL plugin matches broker mechanism list. Broker supports: %s", buffer);
        amqp_deallocate_print_buffer(connection->context, buffer);
    }
}
static void transition_to_waiting_on_sasl_mechanisms(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WaitingOnSaslMechanisms");
    connection->state.sasl.messages.mechanisms = sasl_mechanisms_while_waiting_on_sasl_mechanisms;
    trace_transition(old_state_name);
}

static void transition_to_failed(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "Failed");
    trace_transition(old_state_name);
}

static void done_while_sending_header_response(amqp_connection_t *connection)
{
    // server need to send sasl mechanisms frame
    transition_to_negotiated(connection);
    call_action(connection->state.connection.done, connection->context, connection);
}
static void transition_to_sending_header_response(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "SendingHeader");
    connection->state.sasl.done = done_while_sending_header_response;
    trace_transition(old_state_name);
}

static void transition_to_negotiated(amqp_connection_t *connection) // wrong name
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
            "Connection sasl tunnel does not support \"%s\" while \"%s\" and connection is \"%s\".",
            event, connection->state.sasl.name, connection->state.connection.name);
    amqp_fatal_program_error("Connection sasl state error");
}

static void default_connect(amqp_connection_t *connection)
{
    illegal_state(connection, "Connect");
}

static void default_done(amqp_connection_t *connection)
{
    illegal_state(connection, "Done");
}

static void default_tunnel_establish(amqp_connection_t *connection, uint32_t version)
{
    illegal_state(connection, "TunnelEstablish");
}

static void default_sasl_mechanisms(amqp_connection_t *connection, amqp_frame_t *frame)
{
    illegal_state(connection, "SaslMechanisms");
}

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name)
{
    connection->state.sasl.connect = default_connect;
    connection->state.sasl.done = default_done;
    connection->state.sasl.tunnel.accept = default_tunnel_establish;
    connection->state.sasl.messages.mechanisms = default_sasl_mechanisms;
    connection->state.sasl.name = new_state_name;
}
