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

#include "Transport/Frame/Frame.h"
#include "Transport/Sasl/SaslMechanisms.h"
#include "Transport/Sasl/ClientSasl.h"
#include "Transport/Sasl/ServerSasl.h"
#include "Plugin/Sasl.h"

DECLARE_TRANSITION_FUNCTION(initialized);
DECLARE_TRANSITION_FUNCTION(failed);
DECLARE_TRANSITION_FUNCTION(waiting_on_sasl_mechanisms);
DECLARE_TRANSITION_FUNCTION(sending_header_response);
DECLARE_TRANSITION_FUNCTION(negotiated);
DECLARE_TRANSITION_FUNCTION(waiting_on_sasl_challenge_or_outcome);
DECLARE_TRANSITION_FUNCTION(waiting_on_sasl_initial_response_frame);
DECLARE_TRANSITION_FUNCTION(waiting_on_sasl_challenge_response_frame);
DECLARE_TRANSITION_FUNCTION(waiting_on_sasl_challenge_response_frame);

void amqp_connection_sasl_initialize(amqp_connection_t *connection)
{
    transition_to_state(connection, initialized);
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

    amqp_frame_cleanup(connection->context, frame);

    // TODO - consider if fail is the right action
    connection->state.connection.fail(connection);
}

static
int process_frame(amqp_connection_t *connection, amqp_frame_t *frame, int (*process_method)(amqp_connection_t *connection, amqp_frame_t *frame))
{
    int rc = process_method(connection, frame);
    amqp_frame_cleanup(connection->context, frame);
    return rc;
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
    transition_to_state(connection, waiting_on_sasl_mechanisms);

    // TODO - no longer - connection->state.connection.done(connection);
    
    connection->state.frame.enable(connection);
    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
}
static void sasl_version_rejected_callback(amqp_connection_t *connection, uint32_t version)
{
//    amqp_connection_trace(connection, "SASL version rejected");
    transition_to_state(connection, failed);
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
        transition_to_state(connection, failed);
        amqp_connection_trace(connection, "requested sasl version: %08x, supported version: %08x", requested_version, supported_version);
        call_action(connection->state.connection.mode.server.reject, connection->context, connection, supported_version);
        return;
    }

    transition_to_state(connection, sending_header_response);
    call_action(connection->state.negotiator.send, connection->context, connection, supported_version, sasl_broker_version_sent_callback);
}
DEFINE_TRANSITION_TO_STATE(initialized)
{
    connection->state.sasl.connect = sasl_connect_while_initialized;
    connection->state.sasl.tunnel.accept = tunnel_accept_while_initialized;
}

static void received_sasl_mechanisms_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    if (process_frame(connection, frame, amqp_sasl_process_mechanisms_frame))
    {
        transition_to_state(connection, waiting_on_sasl_challenge_or_outcome);
        connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
    }
}
DEFINE_TRANSITION_TO_STATE(waiting_on_sasl_mechanisms)
{
    connection->state.sasl.frame.mechanisms = received_sasl_mechanisms_frame;
}

//static void wait_for_sasl_challenge_or_outcome(amqp_connection_t *connection)
//{
//    transition_to_state(connection, waiting_on_sasl_challenge_or_outcome);
//    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
//}

//static void done_while_writing_sasl_init_frame(amqp_connection_t *connection)
//{
//    wait_for_sasl_challenge_or_outcome(connection);
//}
//static void transition_to_writing_sasl_init_frame(amqp_connection_t *connection)
//{
//    save_old_state();
//    default_state_initialization(connection, "WritingSaslInitFrame");
//    connection->state.sasl.done = done_while_writing_sasl_init_frame;
//    trace_transition(old_state_name);
//}

static void received_sasl_challenge_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    if (process_frame(connection, frame, amqp_sasl_process_challenge_frame))
    {
        transition_to_state(connection, waiting_on_sasl_challenge_or_outcome);
        connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
//        transition_to_state(connection, writing_sasl_challenge_response_frame);
//        connection->state.writer.commence_write_old(connection, connection->buffer.write, write_complete_callback);
    }
}
static void received_sasl_outcome_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    if (process_frame(connection, frame, amqp_sasl_process_outcome_frame))
    {
        transition_to_state(connection, negotiated);
        call_action(connection->state.connection.done, connection->context, connection);
    }
}
DEFINE_TRANSITION_TO_STATE(waiting_on_sasl_challenge_or_outcome)
{
    connection->state.sasl.frame.challenge = received_sasl_challenge_frame;
    connection->state.sasl.frame.outcome = received_sasl_outcome_frame;
}

//static void done_while_writing_sasl_challenge_response_frame(amqp_connection_t *connection)
//{
//    transition_to_state(connection, waiting_on_sasl_challenge_or_outcome);
//
//    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
//}
//static void transition_to_writing_sasl_challenge_response_frame(amqp_connection_t *connection)
//{
//    save_old_state();
//    default_state_initialization(connection, "WritingSaslChallengeResponseFrame");
//    connection->state.sasl.done = done_while_writing_sasl_challenge_response_frame;
//    trace_transition(old_state_name);
//}

/*************************
 Server states
************************/
static void done_while_sending_header_response(amqp_connection_t *connection)
{
    int rc = amqp_sasl_send_mechanisms_frame(connection);
    if (rc)
    {
        transition_to_state(connection, waiting_on_sasl_initial_response_frame);
        connection->state.frame.enable(connection);
        connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
//        transition_to_state(connection, writing_sasl_mechanisms_frame);
//        connection->state.writer.commence_write_old(connection, connection->buffer.write, write_complete_callback);
    }
}
DEFINE_TRANSITION_TO_STATE(sending_header_response)
{
    connection->state.sasl.done = done_while_sending_header_response;
}

//static void done_while_writing_sasl_mechanisms_frame(amqp_connection_t *connection)
//{
//    transition_to_state(connection, waiting_on_sasl_initial_response_frame);
//    connection->state.frame.enable(connection);
//    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
//}
//static void transition_to_writing_sasl_mechanisms_frame(amqp_connection_t *connection)
//{
//    save_old_state();
//    default_state_initialization(connection, "WritingSaslMechanismsFrame");
//    connection->state.sasl.done = done_while_writing_sasl_mechanisms_frame;
//    trace_transition(old_state_name);
//}

static void received_sasl_initial_response_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    int rc = process_frame(connection, frame, amqp_sasl_process_init_frame);
    switch (rc)
    {
    case amqp_sasl_frame_process_error:
        // Could not process the initial response frame or could not encode the reply
        return;

    case amqp_plugin_handler_outcome_accepted:
        transition_to_state(connection, negotiated);
        call_action(connection->state.connection.done, connection->context, connection);
//        transition_to_state(connection, writing_sasl_accepted_outcome_frame);
//        connection->state.writer.commence_write_old(connection, connection->buffer.write, write_complete_callback);
        break;

    case amqp_plugin_handler_challenge:
        transition_to_state(connection, waiting_on_sasl_challenge_response_frame);
        connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
//        transition_to_state(connection, writing_sasl_challenge_frame);
//        connection->state.writer.commence_write_old(connection, connection->buffer.write, write_complete_callback);
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
DEFINE_TRANSITION_TO_STATE(waiting_on_sasl_initial_response_frame)
{
    connection->state.sasl.frame.init = received_sasl_initial_response_frame;
}

//static void done_while_writing_sasl_challenge_frame(amqp_connection_t *connection)
//{
//    transition_to_state(connection, waiting_on_sasl_challenge_response_frame);
//    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
//}
//static void transition_to_writing_sasl_challenge_frame(amqp_connection_t *connection)
//{
//    save_old_state();
//    default_state_initialization(connection, "WritingSaslChallengeFrame");
//    connection->state.sasl.done = done_while_writing_sasl_challenge_frame;
//    trace_transition(old_state_name);
//}

static void received_sasl_challenge_response_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
SOUTS("received_sasl_challenge_response_frame");
    amqp_sasl_process_challenge_response_frame(connection, frame);
    not_implemented(todo);
}
DEFINE_TRANSITION_TO_STATE(waiting_on_sasl_challenge_response_frame)
{
    connection->state.sasl.frame.init = received_sasl_challenge_response_frame;
}

//static void done_while_writing_sasl_accepted_outcome_frame(amqp_connection_t *connection)
//{
//    transition_to_state(connection, negotiated);
//    call_action(connection->state.connection.done, connection->context, connection);
//}
//static void transition_to_writing_sasl_accepted_outcome_frame(amqp_connection_t *connection)
//{
//    save_old_state();
//    default_state_initialization(connection, "WritingSaslOutcomeFrame");
//    connection->state.sasl.done = done_while_writing_sasl_accepted_outcome_frame;
//    trace_transition(old_state_name);
//}
/***********************
 Common States
************************/
DEFINE_TRANSITION_TO_STATE(negotiated)
{
}

DEFINE_TRANSITION_TO_STATE(failed)
{
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

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection) LIBAMQP_TRACE_STATE_ARGS_DECL)
{
    save_old_state(connection->state.sasl.name);

    connection->state.sasl.connect = default_connect;
    connection->state.sasl.done = default_done;
    connection->state.sasl.tunnel.accept = default_tunnel_establish;
    connection->state.sasl.frame.mechanisms = received_out_of_sequence_sasl_frame;
    connection->state.sasl.frame.init = received_out_of_sequence_sasl_frame;
    connection->state.sasl.frame.challenge = received_out_of_sequence_sasl_frame;
    connection->state.sasl.frame.response = received_out_of_sequence_sasl_frame;
    connection->state.sasl.frame.outcome = received_out_of_sequence_sasl_frame;

    connection->state.sasl.name = state_name;

    action_initializer(connection);

    trace_state_transition(AMQP_TRACE_CONNECTION_SASL, connection->state.sasl.name);
}
