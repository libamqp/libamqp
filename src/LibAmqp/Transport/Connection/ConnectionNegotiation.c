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
#include "Transport/Connection/Connections.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/ConnectionTrace.h"
#include "Transport/Connection/Negotiator.h"
#include "debug_helper.h"

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define save_old_state()  const char* old_state_name = connection->state.negotiator.name
#define trace_transition(old_state_name) amqp_connection_trace_transition(connection, AMQP_TRACE_CONNECTION_NEGOTIATION, old_state_name, connection->state.negotiator.name)
#define trace_disconnect(connection, ...) \
if (connection->trace_flags & AMQP_TRACE_DISCONNECTS) \
    _amqp_connection_trace(connection, __FILE__, __LINE__, __VA_ARGS__)

#else
#define save_old_state()
#define trace_transition(old_state_name)
#define trace_disconnect(context, format,  ...)
#endif

static void transition_to_initialized(amqp_connection_t *connection);
static void transition_to_negotiated(amqp_connection_t *connection);
static void transition_to_failed(amqp_connection_t *connection);
static void transition_to_rejected(amqp_connection_t *connection);
static void transition_to_sending_client_version(amqp_connection_t *connection);
static void transition_to_waiting_for_broker_response(amqp_connection_t *connection);
static void transition_to_waiting_on_client_request(amqp_connection_t *connection);
static void transition_to_waiting_to_send_server_response(amqp_connection_t *connection);
static void transition_to_writing_server_response(amqp_connection_t *connection);

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name);

void amqp_connection_negotiator_initialize(amqp_connection_t *connection)
{
    transition_to_initialized(connection);
}

static void cleanup_resources(amqp_connection_t *connection)
{
}
void amqp_connection_negotiator_cleanup(amqp_connection_t *connection)
{
    cleanup_resources(connection);
}

static void transition_to_terminal_state(amqp_connection_t *connection, void (*transition)(amqp_connection_t *connection))
{
    transition(connection);
    cleanup_resources(connection);
}

int amqp_connection_negotiator_is_state(const amqp_connection_t *connection, const char *state_name)
{
    return connection->state.negotiator.name != 0 ? (strcmp(connection->state.negotiator.name, state_name) == 0) : false;
}

static void read_complete(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    connection->state.negotiator.read_done(connection, buffer, amount);
}
static void write_complete(amqp_connection_t *connection)
{
    connection->state.negotiator.done(connection);
}

// Client sends it's preferred protocol version.
static void start_while_initialized(amqp_connection_t *connection, uint32_t version, amqp_connection_action_f done_callback, amqp_connection_negotiate_callback_f reject_callback)
{
    // Send client preferred protocol version to the broker
    transition_to_sending_client_version(connection);

    connection->data.handshake.preferred_version = version;
    connection->data.handshake.callbacks.confirm = done_callback;
    connection->data.handshake.callbacks.reject = reject_callback;
    connection->data.handshake.callbacks.request = 0;

    amqp_buffer_reset(connection->buffer.write);
    amqp_negotiator_encode_version(connection->context, version, connection->buffer.write);

    connection->state.writer.commence_write(connection, connection->buffer.write, write_complete);
}

// Server to start read while waiting for client's preferred version
static void wait_while_initialized(amqp_connection_t *connection, amqp_connection_negotiate_callback_f request_callback)
{
    transition_to_waiting_on_client_request(connection);

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
static void transition_to_initialized(amqp_connection_t *connection)
{
    default_state_initialization(connection, "Initialized");
    connection->state.negotiator.start = start_while_initialized;
    connection->state.negotiator.wait = wait_while_initialized;
    connection->state.negotiator.reset = reset_while_initialized;

    trace_transition("Created");
}

// Client's preferred version has been sent so switch to waiting for the servers response
static void write_done_while_sending_client_version(amqp_connection_t *connection)
{
    transition_to_waiting_for_broker_response(connection);
    amqp_buffer_reset(connection->buffer.read);
    connection->state.reader.commence_read(connection, connection->buffer.read, AMQP_FRAME_HEADER_SIZE, read_complete);
}
static void transition_to_sending_client_version(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "SendingClientVersion");
    connection->state.negotiator.done = write_done_while_sending_client_version;
    trace_transition(old_state_name);
}

// Client has received the broker's response
static void read_done_while_waiting_for_broker_response(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    uint32_t broker_version;

    assert(buffer == connection->buffer.read);

    if (amount == 0)
    {
        trace_disconnect(connection, "broker closed connection.");
        transition_to_terminal_state(connection, transition_to_failed);
        connection->state.connection.shutdown(connection);
        return;
    }

    broker_version = amqp_negotiator_decode_version(connection->context, buffer);

    if (broker_version == connection->data.handshake.preferred_version)
    {
        transition_to_terminal_state(connection, transition_to_negotiated);
        connection->data.handshake.callbacks.confirm(connection);
    }
    else
    {
        transition_to_terminal_state(connection, transition_to_rejected);
        connection->data.handshake.callbacks.reject(connection, broker_version);
    }
}
static void transition_to_waiting_for_broker_response(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WaitingOnBrokerResponse");
    connection->state.negotiator.read_done = read_done_while_waiting_for_broker_response;
    trace_transition(old_state_name);
}

// Broker has received request from the client
static void read_done_while_waiting_on_client_request(amqp_connection_t *connection, amqp_buffer_t *buffer, int amount)
{
    uint32_t requested_protocol_version;

    assert(buffer ==  connection->buffer.read);

    if (amount == 0)
    {
        trace_disconnect(connection, "client closed connection");
        transition_to_terminal_state(connection, transition_to_failed);
        connection->state.connection.shutdown(connection);
        return;
    }

    requested_protocol_version = amqp_negotiator_decode_version(connection->context, buffer);

    transition_to_waiting_to_send_server_response(connection);

    connection->data.handshake.callbacks.request(connection, requested_protocol_version);
}
static void transition_to_waiting_on_client_request(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WaitingOnClientRequest");
    connection->state.negotiator.read_done = read_done_while_waiting_on_client_request;
    trace_transition(old_state_name);
}

// Send the broker response
static void send_while_waiting_to_send_server_response(amqp_connection_t *connection, uint32_t version, amqp_connection_action_f done_callback)
{
    transition_to_writing_server_response(connection);

    connection->data.handshake.preferred_version = version;
    connection->data.handshake.callbacks.confirm = done_callback;
    connection->data.handshake.callbacks.reject = 0;
    connection->data.handshake.callbacks.request = 0;

    amqp_buffer_reset(connection->buffer.write);
    amqp_negotiator_encode_version(connection->context, version, connection->buffer.write);
    connection->state.writer.commence_write(connection, connection->buffer.write, write_complete);
}
static void transition_to_waiting_to_send_server_response(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WaitToSendServerResponse");
    connection->state.negotiator.send = send_while_waiting_to_send_server_response;
    trace_transition(old_state_name);
}

static void write_done_while_writing_server_response(amqp_connection_t *connection)
{
    // Back to initialized for the next negotiation
    transition_to_terminal_state(connection, transition_to_negotiated);

    connection->data.handshake.callbacks.confirm(connection);
}
static void transition_to_writing_server_response(amqp_connection_t *connection)
{
    save_old_state();

    default_state_initialization(connection, "WritingServerResponse");
    connection->state.negotiator.done = write_done_while_writing_server_response;

    trace_transition(old_state_name);
}

static void reset_while_negotiated(amqp_connection_t *connection)
{
    transition_to_initialized(connection);
}
static void transition_to_negotiated(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "Negotiated");
    connection->state.negotiator.reset = reset_while_negotiated;
    trace_transition(old_state_name);
}

static void transition_to_failed(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "NegotiationFailed");
    trace_transition(old_state_name);
}
static void transition_to_rejected(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "NegotiationRejected");
    trace_transition(old_state_name);
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

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name)
{
    connection->state.negotiator.start = default_start;
    connection->state.negotiator.wait = default_wait;
    connection->state.negotiator.send = default_send;
    connection->state.negotiator.read_done = default_read_done;
    connection->state.negotiator.done = default_done;
    connection->state.negotiator.reset = default_reset;

    connection->state.negotiator.name = new_state_name;
}
