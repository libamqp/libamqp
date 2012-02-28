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

#include "Transport/Frame/Frame.h"
#include "Transport/Amqp/AmqpFrames.h"
#include "Transport/Connection/ConnectionStateMachine.h"

DECLARE_TRANSITION_FUNCTION(initialized);
DECLARE_TRANSITION_FUNCTION(open_sent);
DECLARE_TRANSITION_FUNCTION(opened);
DECLARE_TRANSITION_FUNCTION(waiting_for_open_frame);
DECLARE_TRANSITION_FUNCTION(close_sent);
DECLARE_TRANSITION_FUNCTION(closed);

void amqp_connection_amqp_initialize(amqp_connection_t *connection)
{
    transition_to_state(connection, initialized);
}

static void cleanup_resources(amqp_connection_t *connection)
{
}
void amqp_connection_amqp_cleanup(amqp_connection_t *connection)
{
    cleanup_resources(connection);
}

int amqp_connection_amqp_is_state(const amqp_connection_t *connection, const char *state_name)
{
    return connection->state.amqp.name != 0 ? (strcmp(connection->state.amqp.name, state_name) == 0) : false;
}

// start the idle timeouts with the longest delay that will give a reasonably accurate timeout check time.
static void start_idle_timeouts(amqp_connection_t *connection)
{
    ev_tstamp local = connection->amqp.connection.timeout.local = connection->amqp.connection.limits.idle_time_out / 1000.0;
    ev_tstamp remote = connection->amqp.connection.timeout.remote = connection->amqp.connection.remote_limits.idle_time_out / 2000.0;
    ev_tstamp delay = connection->amqp.connection.timeout.period = (local < remote ? local : remote) / 2.0;

    amqp_timer_start(connection->context, connection->timer, delay);
}

void amqp_received_out_of_sequence_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_connection_error(connection, AMQP_ERROR_OUT_OF_SEQUENCE_AMQP_FRAME,
                "Received an out of sequence AMQP frame. Frame id: %llu while state is\"%s\" and connection is \"%s\". Aborting connection.",
                (unsigned long long) amqp_frame_descriptor_full_id(frame), connection->state.sasl.name, connection->state.connection.name);

    amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_AMQP_ERROR | AMQP_CONNECTION_FRAME_SEQUENCE_ERROR);
    amqp_frame_cleanup(connection->context, frame);
    connection->state.connection.shutdown(connection);
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
    amqp_frame_t *frame = amqp_decode_amqp_frame(connection->context, buffer);
    if (frame)
    {
        amqp_trace_decoded_frame(connection, frame);
        frame->dispatch(connection, frame);
    }
    else
    {
        amqp_connection_error(connection, AMQP_ERROR_FRAME_DECODE_FAILED, "Could not decode what should be an AMQP frame.");
        amqp_connection_failure_flag_set(connection, AMQP_CONNECTION_AMQP_ERROR | AMQP_CONNECTION_AMQP_FRAME_DECODE_ERROR);
        connection->state.connection.shutdown(connection);
    }
}
static inline
void read_frame(amqp_connection_t *connection)
{
    connection->state.frame.read(connection, connection->buffer.read, frame_available_callback);
}

static
void send_open_while_initialized(amqp_connection_t *connection)
{
    int rc =  amqp_send_amqp_open_frame(connection);
    if (rc)
    {
        transition_to_state(connection, open_sent);
        connection->state.frame.enable(connection);
        read_frame(connection);
    }
}
static
void wait_on_open_while_initialized(amqp_connection_t *connection)
{
    transition_to_state(connection, waiting_for_open_frame);
    connection->state.frame.enable(connection);
    read_frame(connection);
}
DEFINE_TRANSITION_TO_STATE(initialized)
{
    connection->state.amqp.send_open = send_open_while_initialized;
    connection->state.amqp.wait_on_open = wait_on_open_while_initialized;
}

static amqp_session_t *create_session(amqp_connection_t *connection)
{
not_implemented(todo);
    amqp_session_t *result;
    int channel = amqp_connection_next_channel(connection);
    if (channel == -1)
    {
        return 0;
    }

    result = AMQP_MALLOC(connection->context, amqp_session_t);
    connection->sessions.sessions[channel] = result;

    amqp_session_initialize(connection, result, channel);
    amqp_send_amqp_begin_frame(connection, result);

    return result;
}

/**********************************************
 Client states
 *********************************************/
static void connection_opened(amqp_connection_t *connection)
{
    flag_amqp_connection_opened(connection);
    transition_to_state(connection, opened);
    start_idle_timeouts(connection);
    read_frame(connection);
}
static void open_received_after_open_sent(amqp_connection_t *connection, amqp_frame_t *frame)
{
    if (process_frame(connection, frame, amqp_process_open_frame_from_broker))
    {
        connection_opened(connection);
    }
}
static void keep_alive_while_open_sent(amqp_connection_t *connection, amqp_frame_t *frame)
{
    read_frame(connection);
}
DEFINE_TRANSITION_TO_STATE(open_sent)
{
    connection->state.amqp.frame.open = open_received_after_open_sent;
    connection->state.amqp.frame.empty = keep_alive_while_open_sent;

    // TODO  - caution - creating a session that may not be usable
    connection->state.amqp.create_session = create_session;
}

/**********************************************
 Server states
 *********************************************/
static void open_received_while_waiting_for_open_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    int rc = process_frame(connection, frame, amqp_process_open_frame_from_client);
    if (!rc)
    {
        return;
    }

    rc =  amqp_send_amqp_open_frame(connection);
    if (rc);
    {
        connection_opened(connection);
    }
}
DEFINE_TRANSITION_TO_STATE(waiting_for_open_frame)
{
    connection->state.amqp.frame.open = open_received_while_waiting_for_open_frame;
}


/**********************************************
 Common states
 *********************************************/

static void discard_frame(amqp_connection_t *connection);
static
void discard_frame_after_close_received(amqp_connection_t *connection, amqp_buffer_t *buffer)
{
    discard_frame(connection);
}
static inline
void discard_frame(amqp_connection_t *connection)
{
    connection->state.frame.read(connection, connection->buffer.read, discard_frame_after_close_received);
}

static void begin_connection_close(amqp_connection_t *connection)
{
    int rc =  amqp_send_amqp_close_frame(connection);
    if (rc)
    {
        amqp_timer_start(connection->context, connection->timer, 3.0);  // TODO - remove magic no and put in config
        transition_to_state(connection, close_sent);
        flag_amqp_connection_closing(connection);
        read_frame(connection);
    }
}
static void application_close_while_opened(amqp_connection_t *connection)
{
    begin_connection_close(connection);
}
static void timeout_expired_while_opened(amqp_connection_t *connection)
{
    ev_tstamp now = ev_now(connection->context->thread_event_loop);

    if ((connection->io.read.last_read_time - now + connection->amqp.connection.timeout.local) < 0)
    {
        // TODO - include error details in the close frame
        begin_connection_close(connection);
        return;
    }

    if ((connection->io.write.last_write_time - now + connection->amqp.connection.timeout.remote) < 0)
    {
        amqp_send_empty_frame(connection);
        return;
    }
}
static void received_close_while_opened(amqp_connection_t *connection, amqp_frame_t *frame)
{
    int rc;

    amqp_timer_start(connection->context, connection->timer, 3.0);  // TODO - remove magic no and put in config

    flag_amqp_connection_closed(connection);
    transition_to_state(connection, closed);
    process_frame(connection, frame, amqp_process_close_frame);
    discard_frame(connection);                                       // TODO -  rename
    rc = amqp_send_amqp_close_frame(connection);
    if (rc)
    {
        connection->state.shutdown.drain(connection);
        return;
    }
    connection->state.shutdown.close(connection);
}
static void keep_alive_while_opened(amqp_connection_t *connection, amqp_frame_t *frame)
{
    read_frame(connection);
}
//static void amqp_begin_while_opened(amqp_connection_t *connection, amqp_frame_t *frame)
//{
//    int rc = process_frame(connection, frame, amqp_process_begin_frame);
//    if (rc)
//    {
//        read_frame(connection);
//    }
//}
DEFINE_TRANSITION_TO_STATE(opened)
{
    connection->state.connection.shutdown = application_close_while_opened;
    connection->state.connection.timeout = timeout_expired_while_opened;
    connection->state.amqp.frame.close = received_close_while_opened;
    connection->state.amqp.frame.empty = keep_alive_while_opened;
    connection->state.amqp.create_session = create_session;

//    connection->state.amqp.frame.begin = amqp_begin_while_opened;
}

static void received_close_after_close_sent(amqp_connection_t *connection, amqp_frame_t *frame)
{
    int rc = process_frame(connection, frame, amqp_process_close_frame);
    if (rc)
    {
        flag_amqp_connection_closed(connection);
        transition_to_state(connection, closed);
        connection->state.shutdown.drain(connection);
        return;
    }
    connection->state.shutdown.close(connection);
}
static void timeout_expiry_while_close_sent(amqp_connection_t *connection)
{
    flag_amqp_connection_closed(connection);
    transition_to_state(connection, closed);
    connection->state.shutdown.drain(connection);
}
DEFINE_TRANSITION_TO_STATE(close_sent)
{
    connection->state.connection.shutdown = amqp__shutdown_while_in_progress;
    connection->state.connection.timeout = timeout_expiry_while_close_sent;
    connection->state.amqp.frame.close = received_close_after_close_sent;
    // TODO -  ignore link extabllist, attacj, begin
}

static void timeout_expiry_while_closed(amqp_connection_t *connection)
{
    connection->state.shutdown.close(connection);
}
DEFINE_TRANSITION_TO_STATE(closed)
{
    connection->state.connection.timeout = timeout_expiry_while_closed;
    connection->state.connection.shutdown = amqp__shutdown_while_in_progress;
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

static void default_send_open(amqp_connection_t *connection)
{
    illegal_state(connection, "SendOpen");
}

static void default_wait_on_open(amqp_connection_t *connection)
{
    illegal_state(connection, "WaitOnOpen");
}

static amqp_session_t *default_create_session(amqp_connection_t *connection)
{
    return 0;
}
static void default_state_initialization(amqp_connection_t *connection, const char *state_name, void (*action_initializer)(amqp_connection_t *connection) LIBAMQP_TRACE_STATE_ARGS_DECL)
{
    save_old_state(connection->state.amqp.name);

    connection->state.amqp.send_open = default_send_open;
    connection->state.amqp.wait_on_open = default_wait_on_open;
    connection->state.amqp.send_open = default_send_open;
    connection->state.amqp.create_session = default_create_session;

    connection->state.amqp.frame.open = amqp_received_out_of_sequence_frame;
    connection->state.amqp.frame.begin = amqp_received_out_of_sequence_frame;
    connection->state.amqp.frame.attach = amqp_received_out_of_sequence_frame;
    connection->state.amqp.frame.flow = amqp_received_out_of_sequence_frame;
    connection->state.amqp.frame.transfer = amqp_received_out_of_sequence_frame;
    connection->state.amqp.frame.disposition = amqp_received_out_of_sequence_frame;
    connection->state.amqp.frame.detach = amqp_received_out_of_sequence_frame;
    connection->state.amqp.frame.end = amqp_received_out_of_sequence_frame;
    connection->state.amqp.frame.close = amqp_received_out_of_sequence_frame;
    connection->state.amqp.frame.empty = amqp_received_out_of_sequence_frame;

    connection->state.amqp.name = state_name;

    action_initializer(connection);

    trace_state_transition(AMQP_TRACE_CONNECTION_AMQP, connection->state.amqp.name);
}
