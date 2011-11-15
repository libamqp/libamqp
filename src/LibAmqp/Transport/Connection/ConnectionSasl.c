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
#include "Transport/Sasl/ClientSasl.h"
#include "Transport/Sasl/ServerSasl.h"
#include "Plugin/Sasl.h"
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
static void transition_to_writing_sasl_init_frame(amqp_connection_t *connection);

static void transition_to_waiting_on_sasl_challenge_or_outcome(amqp_connection_t *connection);
static void transition_to_writing_sasl_challenge_response_frame(amqp_connection_t *connection);

static void transition_to_writing_sasl_mechanisms_frame(amqp_connection_t *connection);
static void transition_to_waiting_on_sasl_initial_response_frame(amqp_connection_t *connection);
static void transition_to_waiting_on_sasl_challenge_response_frame(amqp_connection_t *connection);
static void transition_to_writing_sasl_challenge_frame(amqp_connection_t *connection);
static void transition_to_writing_sasl_accepted_outcome_frame(amqp_connection_t *connection);

static void transition_to_waiting_on_sasl_challenge_response_frame(amqp_connection_t *connection);

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name);

void amqp_connection_sasl_initialize(amqp_connection_t *connection)
{
    transition_to_initialized(connection);
}

static void cleanup_resources(amqp_connection_t *connection)
{
    if (connection->sasl.plugin)
    {
        amqp_sasl_plugin_instance_cleanup(connection->context, connection->sasl.plugin);
        connection->sasl.plugin = 0;
    }
}
void amqp_connection_sasl_cleanup(amqp_connection_t *connection)
{
    cleanup_resources(connection);
}

int amqp_connection_sasl_is_state(const amqp_connection_t *connection, const char *state_name)
{
    return connection->state.sasl.name != 0 ? (strcmp(connection->state.sasl.name, state_name) == 0) : false;
}

static void received_out_of_sequence_sasl_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_connection_error(connection, AMQP_ERROR_OUT_OF_SEQUENCE_SASL_FRAME,
                "Received an out of sequence SASL frame. Frame id: %llu while state is\"%s\" and connection is \"%s\". Aborting connection.",
                (unsigned long long) amqp_frame_descriptor_full_id(frame), connection->state.sasl.name, connection->state.connection.name);

    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_SASL_ERROR | AMQP_CONNECTION_FRAME_SEQUENCE_ERROR);

    // TODO - consider if fail is the right action
    connection->state.connection.fail(connection);
}

static
void write_complete_callback(amqp_connection_t *connection)
{
    connection->state.sasl.done(connection);
}

static
void frame_available_callback(amqp_connection_t *connection, amqp_buffer_t *buffer)
{
    amqp_frame_t *frame = amqp_decode_sasl_frame(connection->context, buffer);
    if (frame)
    {
        amqp_trace_decoded_frame(connection, frame);
        frame->dispatch(connection, frame);
    }
    else
    {
        amqp_connection_failed(connection, AMQP_ERROR_FRAME_DECODE_FAILED, AMQP_CONNECTION_SASL_FRAME_DECODE_ERROR, "Could not decode frame from broker during SASL handshake");
    }
}

static
int process_frame(amqp_connection_t *connection, amqp_frame_t *frame, int rc)
{
    amqp_frame_cleanup(connection->context, frame);
    return rc;
}

static
void sasl_broker_version_sent_callback(amqp_connection_t *connection)
{
    connection->state.sasl.done(connection);
}

static
void sasl_version_accepted_callback(amqp_connection_t *connection)
{
    // The broker has accepted our preferred sasl version and will next send it's supported SASL mechanisms
//    amqp_connection_trace(connection, "SASL version accepted");
    connection->specification_version.supported.sasl = connection->specification_version.required.sasl;
    transition_to_waiting_on_sasl_mechanisms(connection);

    // TODO - no longer - connection->state.connection.done(connection);
    
    connection->state.frame.enable(connection);
    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
}
static void sasl_version_rejected_callback(amqp_connection_t *connection, uint32_t version)
{
//    amqp_connection_trace(connection, "SASL version rejected");
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

static void received_sasl_mechanisms_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    if (process_frame(connection, frame,
        amqp_sasl_process_mechanisms_frame(connection, frame)
        ))
    {
        transition_to_writing_sasl_init_frame(connection);
        connection->state.writer.commence_write(connection, connection->buffer.write, write_complete_callback);
    }
}
static void transition_to_waiting_on_sasl_mechanisms(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WaitingOnSaslMechanisms");
    connection->state.sasl.frame.mechanisms = received_sasl_mechanisms_frame;
    trace_transition(old_state_name);
}

static void wait_for_sasl_challenge_or_outcome(amqp_connection_t *connection)
{
    transition_to_waiting_on_sasl_challenge_or_outcome(connection);

    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
}

static void done_while_writing_sasl_init_frame(amqp_connection_t *connection)
{
    wait_for_sasl_challenge_or_outcome(connection);
}
static void transition_to_writing_sasl_init_frame(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WritingSaslInitFrame");
    connection->state.sasl.done = done_while_writing_sasl_init_frame;
    trace_transition(old_state_name);
}

static void received_sasl_challenge_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    if (process_frame(connection, frame,
        amqp_sasl_process_challenge_frame(connection, frame)
        ))
    {
        transition_to_writing_sasl_challenge_response_frame(connection);
        connection->state.writer.commence_write(connection, connection->buffer.write, write_complete_callback);
    }
}
static void received_sasl_outcome_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    if (process_frame(connection, frame,
        amqp_sasl_process_outcome_frame(connection, frame)
        ))
    {
        transition_to_negotiated(connection);
        call_action(connection->state.connection.done, connection->context, connection);
    }
}
static void transition_to_waiting_on_sasl_challenge_or_outcome(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WaitingOnSaslChallengeOrOutcomeFrame");
    connection->state.sasl.frame.challenge = received_sasl_challenge_frame;
    connection->state.sasl.frame.outcome = received_sasl_outcome_frame;
    trace_transition(old_state_name);
}

static void done_while_writing_sasl_challenge_response_frame(amqp_connection_t *connection)
{
    transition_to_waiting_on_sasl_challenge_or_outcome(connection);

    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
}
static void transition_to_writing_sasl_challenge_response_frame(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WritingSaslChallengeResponseFrame");
    connection->state.sasl.done = done_while_writing_sasl_challenge_response_frame;
    trace_transition(old_state_name);
}

/*************************
 Server states
************************/
static void done_while_sending_header_response(amqp_connection_t *connection)
{
    int rc = amqp_sasl_prepare_mechanisms_frame(connection);
    if (rc)
    {
        transition_to_writing_sasl_mechanisms_frame(connection);
        connection->state.writer.commence_write(connection, connection->buffer.write, write_complete_callback);
    }
}
static void transition_to_sending_header_response(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "SendingHeader");
    connection->state.sasl.done = done_while_sending_header_response;
    trace_transition(old_state_name);
}

static void done_while_writing_sasl_mechanisms_frame(amqp_connection_t *connection)
{
    transition_to_waiting_on_sasl_initial_response_frame(connection);
    connection->state.frame.enable(connection);
    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
}
static void transition_to_writing_sasl_mechanisms_frame(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WritingSaslMechanismsFrame");
    connection->state.sasl.done = done_while_writing_sasl_mechanisms_frame;
    trace_transition(old_state_name);
}

static void received_sasl_initial_response_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    int rc = amqp_sasl_process_init_frame(connection, frame);
    amqp_frame_cleanup(connection->context, frame);
    switch (rc)
    {
    case amqp_plugin_handler_outcome_accepted:
        transition_to_writing_sasl_accepted_outcome_frame(connection);
        connection->state.writer.commence_write(connection, connection->buffer.write, write_complete_callback);
        break;

    case amqp_plugin_handler_challenge:
        transition_to_writing_sasl_challenge_frame(connection);
        connection->state.writer.commence_write(connection, connection->buffer.write, write_complete_callback);
        not_implemented(todo);
        break;

    case amqp_plugin_handler_outcome_rejected:
        not_implemented(todo);

    case amqp_plugin_handler_failed:
        not_implemented(todo);

    default:
        abort();
    }
}
// After receiving initial response from the client the broker can send a challenge or outcome frame
static void transition_to_waiting_on_sasl_initial_response_frame(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WaitingOnSaslInitialResponseFrame");
    connection->state.sasl.frame.init = received_sasl_initial_response_frame;
    trace_transition(old_state_name);
}

static void done_while_writing_sasl_challenge_frame(amqp_connection_t *connection)
{
    transition_to_waiting_on_sasl_challenge_response_frame(connection);
    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
}
static void transition_to_writing_sasl_challenge_frame(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WritingSaslChallengeFrame");
    connection->state.sasl.done = done_while_writing_sasl_challenge_frame;
    trace_transition(old_state_name);
}

static void received_sasl_challenge_response_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
SOUTS("received_sasl_challenge_response_frame");
    amqp_sasl_process_challenge_response_frame(connection, frame);
    not_implemented(todo);
}
static void transition_to_waiting_on_sasl_challenge_response_frame(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WaitingOnSaslInitialResponseFrame");
    connection->state.sasl.frame.init = received_sasl_challenge_response_frame;
    trace_transition(old_state_name);
}

static void done_while_writing_sasl_accepted_outcome_frame(amqp_connection_t *connection)
{
    transition_to_negotiated(connection);
    call_action(connection->state.connection.done, connection->context, connection);
}
static void transition_to_writing_sasl_accepted_outcome_frame(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WritingSaslOutcomeFrame");
    connection->state.sasl.done = done_while_writing_sasl_accepted_outcome_frame;
    trace_transition(old_state_name);
}
/***********************
 Common States
************************/
static void transition_to_negotiated(amqp_connection_t *connection) // wrong name
{
    save_old_state();
    default_state_initialization(connection, "Negotiated");
    trace_transition(old_state_name);
}

static void transition_to_failed(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "Failed");
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

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name)
{
    connection->state.sasl.connect = default_connect;
    connection->state.sasl.done = default_done;
    connection->state.sasl.tunnel.accept = default_tunnel_establish;
    connection->state.sasl.frame.mechanisms = received_out_of_sequence_sasl_frame;
    connection->state.sasl.frame.init = received_out_of_sequence_sasl_frame;
    connection->state.sasl.frame.challenge = received_out_of_sequence_sasl_frame;
    connection->state.sasl.frame.response = received_out_of_sequence_sasl_frame;
    connection->state.sasl.frame.outcome = received_out_of_sequence_sasl_frame;
    connection->state.sasl.name = new_state_name;
}
