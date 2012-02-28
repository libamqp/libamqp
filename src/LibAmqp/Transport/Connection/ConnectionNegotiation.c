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
//#include "Transport/Connection/Negotiator.h"

DECLARE_TRANSITION_FUNCTION(initialized);
DECLARE_TRANSITION_FUNCTION(negotiated);
DECLARE_TRANSITION_FUNCTION(failed);
DECLARE_TRANSITION_FUNCTION(rejected);
DECLARE_TRANSITION_FUNCTION(waiting_for_broker_response);
DECLARE_TRANSITION_FUNCTION(waiting_on_client_request);
DECLARE_TRANSITION_FUNCTION(waiting_to_send_server_response);

void amqp_connection_negotiator_initialize(amqp_connection_t *connection)
{
    transition_to_state(connection, initialized);
}

void amqp_connection_negotiator_cleanup(amqp_connection_t *connection)
{
}

//static void transition_to_terminal_state(amqp_connection_t *connection, void (*transition)(amqp_connection_t *connection))
//{
//    transition(connection);
//    cleanup_resources(connection);
//}

int amqp_connection_negotiator_is_state(const amqp_connection_t *connection, const char *state_name)
{
    return connection->state.negotiator.name != 0 ? (strcmp(connection->state.negotiator.name, state_name) == 0) : false;
}

static void read_complete(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    connection->state.negotiator.read_done(connection, buffer, amount);
}

static void write_version(amqp_connection_t *connection, uint32_t version)
{
    amqp_buffer_t *buffer = amqp_allocate_buffer(connection->context);
    amqp_negotiator_encode_version(connection->context, version, buffer);
    amqp_connection_write_buffer(connection, buffer);
}

// Client sends it's preferred protocol version.
static void start_while_initialized(amqp_connection_t *connection, uint32_t version, amqp_connection_action_f done_callback, amqp_connection_negotiate_callback_f reject_callback)
{
    // Send client preferred protocol version to the broker
    connection->data.handshake.preferred_version = version;
    connection->data.handshake.callbacks.confirm = done_callback;
    connection->data.handshake.callbacks.reject = reject_callback;
    connection->data.handshake.callbacks.request = 0;

    write_version(connection, version);

    transition_to_state(connection, waiting_for_broker_response);
    amqp_buffer_reset(connection->buffer.read);
    connection->state.reader.commence_read(connection, connection->buffer.read, AMQP_FRAME_HEADER_SIZE, read_complete);
}

// Server to start read while waiting for client's preferred version
static void wait_while_initialized(amqp_connection_t *connection, amqp_connection_negotiate_callback_f request_callback)
{
    transition_to_state(connection, waiting_on_client_request);

    connection->data.handshake.preferred_version = 0;
    connection->data.handshake.callbacks.confirm = 0;
    connection->data.handshake.callbacks.reject = 0;
    connection->data.handshake.callbacks.request = request_callback;

    connection->state.reader.commence_read(connection, connection->buffer.read, AMQP_FRAME_HEADER_SIZE, read_complete);
}
static void reset_while_initialized(amqp_connection_t *connection)
{
    // Do nothing
}
DEFINE_TRANSITION_TO_STATE(initialized)
{
    connection->state.negotiator.start = start_while_initialized;
    connection->state.negotiator.wait = wait_while_initialized;
    connection->state.negotiator.reset = reset_while_initialized;
}

// Client has received the broker's response
static void read_done_while_waiting_for_broker_response(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    uint32_t broker_version;

    assert(buffer == connection->buffer.read);

    if (amount == 0)
    {
        trace_disconnect(connection, "broker closed connection.");
        transition_to_state(connection, failed);
        connection->state.connection.shutdown(connection);
        return;
    }

    broker_version = amqp_negotiator_decode_version(connection->context, buffer);

    if (broker_version == connection->data.handshake.preferred_version)
    {
        transition_to_state(connection, negotiated);
        connection->data.handshake.callbacks.confirm(connection);
    }
    else
    {
        transition_to_state(connection, rejected);
        connection->data.handshake.callbacks.reject(connection, broker_version);
    }
}
DEFINE_TRANSITION_TO_STATE(waiting_for_broker_response)
{
    connection->state.negotiator.read_done = read_done_while_waiting_for_broker_response;
}

// Broker has received request from the client
static void read_done_while_waiting_on_client_request(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    uint32_t requested_protocol_version;

    assert(buffer ==  connection->buffer.read);

    if (amount == 0)
    {
        trace_disconnect(connection, "client closed connection");
        transition_to_state(connection, failed);
        connection->state.connection.shutdown(connection);
        return;
    }

    requested_protocol_version = amqp_negotiator_decode_version(connection->context, buffer);

    transition_to_state(connection, waiting_to_send_server_response);

    connection->data.handshake.callbacks.request(connection, requested_protocol_version);
}
DEFINE_TRANSITION_TO_STATE(waiting_on_client_request)
{
    connection->state.negotiator.read_done = read_done_while_waiting_on_client_request;
}

// Send the broker response
static void send_while_waiting_to_send_server_response(amqp_connection_t *connection, uint32_t version, amqp_connection_action_f done_callback)
{
    connection->data.handshake.preferred_version = version;
    connection->data.handshake.callbacks.confirm = done_callback;
    connection->data.handshake.callbacks.reject = 0;
    connection->data.handshake.callbacks.request = 0;

    write_version(connection, version);

    // Back to initialized for the next negotiation
    transition_to_state(connection, negotiated);
    connection->data.handshake.callbacks.confirm(connection);
}
DEFINE_TRANSITION_TO_STATE(waiting_to_send_server_response)
{
    connection->state.negotiator.send = send_while_waiting_to_send_server_response;
}

static void reset_while_negotiated(amqp_connection_t *connection)
{
    transition_to_state(connection, initialized);
}
DEFINE_TRANSITION_TO_STATE(negotiated)
{
    connection->state.negotiator.reset = reset_while_negotiated;
}

DEFINE_TRANSITION_TO_STATE(failed)
{
}
DEFINE_TRANSITION_TO_STATE(rejected)
{
}

/**********************************************
 Default states
 *********************************************/
static void illegal_state(amqp_connection_t *connection, const char *event)
{
    amqp_connection_error(connection, AMQP_ERROR_ILLEGAL_STATE,
            "Connection protocol negotiator does not support \"%s\" while \"%s\" and connection is \"%s\".",
            event, connection->state.negotiator.name, connection->state.connection.name);
    amqp_fatal_program_error("Connection negotiation state error");
}

static void default_start(amqp_connection_t *connection, uint32_t version, amqp_connection_action_f done_callback, amqp_connection_negotiate_callback_f reject_callback)
{
    illegal_state(connection, "Start");
}
static void default_wait(amqp_connection_t *connection, amqp_connection_negotiate_callback_f request_callback)
{
    illegal_state(connection, "Wait");
}
static void default_send(amqp_connection_t *connection, uint32_t version, amqp_connection_action_f done_callback)
{
    illegal_state(connection, "Send");
}
static void default_read_done(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    illegal_state(connection, "ReadDone");
}
static void default_done(amqp_connection_t *connection)
{
    illegal_state(connection, "Done");
}
static void default_reset(amqp_connection_t *connection)
{
    illegal_state(connection, "Reset");
}

static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection) LIBAMQP_TRACE_STATE_ARGS_DECL)
{
    save_old_state(connection->state.negotiator.name);

    connection->state.negotiator.start = default_start;
    connection->state.negotiator.wait = default_wait;
    connection->state.negotiator.send = default_send;
    connection->state.negotiator.read_done = default_read_done;
    connection->state.negotiator.done = default_done;
    connection->state.negotiator.reset = default_reset;

    connection->state.negotiator.name = state_name;

    action_initializer(connection);

    trace_state_transition(AMQP_TRACE_CONNECTION_NEGOTIATION, connection->state.negotiator.name);
}
