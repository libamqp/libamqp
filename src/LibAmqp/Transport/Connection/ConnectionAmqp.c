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
#include "Transport/Connection/Connection.h"
#include "Transport/Connection/ConnectionTrace.h"
#include "Transport/Frame/Frame.h"
#include "Transport/Amqp/AmqpFrames.h"

#ifdef LIBAMQP_TRACE_CONNECT_STATE
#define save_old_state()  const char* old_state_name = connection->state.amqp.name
#define trace_transition(old_state_name) amqp_connection_trace_transition(connection, AMQP_TRACE_CONNECTION_AMQP, old_state_name, connection->state.amqp.name)
#else
#define save_old_state()
#define trace_transition(old_state_name)
#endif

static void transition_to_initialized(amqp_connection_t *connection);
static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name);

static void transition_to_writing_open_frame(amqp_connection_t *connection);
static void transition_to_open_sent(amqp_connection_t *connection);
static void transition_to_opened(amqp_connection_t *connection);
static void transition_to_waiting_for_open_frame(amqp_connection_t *connection);
static void transition_to_writing_open_after_open_received(amqp_connection_t *connection);
//static void transition_to_writing_close_frame(amqp_connection_t *connection);
//static void transition_to_close_sent(amqp_connection_t *connection);

void amqp_connection_amqp_initialize(amqp_connection_t *connection)
{
    transition_to_initialized(connection);
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

//static
void shutdown_amqp_tunnel_once_opened(amqp_connection_t *connection)
{
    not_implemented(todo);
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
void write_complete_callback(amqp_connection_t *connection)
{
    connection->state.amqp.done(connection);
}
static inline
void write_frame(amqp_connection_t *connection)
{
    connection->state.writer.commence_write(connection, connection->buffer.write, write_complete_callback);
}

static
void send_open_while_initialized(amqp_connection_t *connection)
{
    int rc = amqp_prepare_open_frame(connection);
    if (rc)
    {
        transition_to_writing_open_frame(connection);
        connection->state.frame.enable(connection);
        write_frame(connection);
    }
}
static
void wait_on_open_while_initialized(amqp_connection_t *connection)
{
    transition_to_waiting_for_open_frame(connection);
    connection->state.frame.enable(connection);
    read_frame(connection);
}
static
void transition_to_initialized(amqp_connection_t *connection)
{
    default_state_initialization(connection, "Initialized");
    connection->state.amqp.send_open = send_open_while_initialized;
    connection->state.amqp.wait_on_open = wait_on_open_while_initialized;
    trace_transition("Created");
}

/**********************************************
 Client states
 *********************************************/
static void done_while_writing_open(amqp_connection_t *connection)
{
    transition_to_open_sent(connection);
    read_frame(connection);
}
static void transition_to_writing_open_frame(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WritingOpen");
    connection->state.amqp.done = done_while_writing_open;
    trace_transition(old_state_name);
}

static void open_received_after_open_sent(amqp_connection_t *connection, amqp_frame_t *frame)
{
    if (process_frame(connection, frame, amqp_process_open_frame))
    {
        transition_to_opened(connection);
        flag_amqp_connection_opened(connection);
        read_frame(connection);
    }
}
static void transition_to_open_sent(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "OpenSent");
    connection->state.amqp.frame.open = open_received_after_open_sent;
    trace_transition(old_state_name);
}

//static void shutdown_while_opened(amqp_connection_t *connection)
//{
//    int rc = amqp_prepare_close_frame(connection);
//    connection->state.connection.shutdown = amqp__shutdown_while_in_progress;
//
//    flag_amqp_connection_closing(connection);
//    if (rc)
//    {
//        transition_to_writing_close_frame(connection);
//        write_frame(connection);
//    }
//}
static void received_close_while_opened(amqp_connection_t *connection, amqp_frame_t *frame)
{
    flag_amqp_connection_closing(connection);
//    amqp_prepare_close_frame(connection);
    not_implemented(todo);
}
static void transition_to_opened(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "Opened");
//    connection->state.connection.shutdown = shutdown_while_opened;
    connection->state.amqp.frame.close = received_close_while_opened;
    trace_transition(old_state_name);
}
/*
static void done_while_writing_close(amqp_connection_t *connection)
{
    transition_to_close_sent(connection);
    read_frame(connection);
}
static void received_close_while_writing_close_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    not_implemented(todo);
}
static void transition_to_writing_close_frame(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WritingCLose");
    connection->state.amqp.done = done_while_writing_close;
    connection->state.amqp.frame.close = received_close_while_writing_close_frame;
    trace_transition(old_state_name);
}

static void received_close_after_close_sent(amqp_connection_t *connection, amqp_frame_t *frame)
{
    int rc = process_frame(connection, frame, amqp_process_close_frame);
    flag_amqp_connection_closed(connection);
    if (rc)
    {
        connection->state.shutdown.drain(connection);
        return;
    }
    connection->state.shutdown.close(connection);
}
static void transition_to_close_sent(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "CloseSent");
    connection->state.amqp.frame.close = received_close_after_close_sent;
    trace_transition(old_state_name);
}
*/
/**********************************************
 Server states
 *********************************************/
static void open_received_while_waiting_for_open_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    if (process_frame(connection, frame, amqp_process_open_frame))
    {
        transition_to_writing_open_after_open_received(connection);
        write_frame(connection);
    }
}
static void transition_to_waiting_for_open_frame(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WaitingOnOpen");
    connection->state.amqp.frame.open = open_received_while_waiting_for_open_frame;
    trace_transition(old_state_name);
}

static void done_while_writing_open_after_open_received(amqp_connection_t *connection)
{
    flag_amqp_connection_opened(connection);
    transition_to_opened(connection);
}
static void transition_to_writing_open_after_open_received(amqp_connection_t *connection)
{
    save_old_state();
    default_state_initialization(connection, "WritingOpenAfterOpenrReceived");
    connection->state.amqp.done = done_while_writing_open_after_open_received;
    trace_transition(old_state_name);
}

//static void close_while_receiver_opened(amqp_connection_t *connection, amqp_frame_t *frame)
//{
//    int rc = amqp_prepare_close_frame(connection);
//    if (rc)
//    {
//        transition_to_writing_close_frame(connection);
//        write_frame(connection);
//    }
//
//    not_implemented(todo);
//}
//static void transition_to_receiver_opened(amqp_connection_t *connection)
//{
//    save_old_state();
//    default_state_initialization(connection, "`ReceiverOpened");
//    connection->state.amqp.frame.close = close_while_receiver_opened;
//    trace_transition(old_state_name);
//}

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

static void default_done(amqp_connection_t *connection)
{
    illegal_state(connection, "Done");
}

static void default_state_initialization(amqp_connection_t *connection, const char *new_state_name)
{
    connection->state.amqp.send_open = default_send_open;
    connection->state.amqp.wait_on_open = default_wait_on_open;
    connection->state.amqp.done = default_done;

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

    connection->state.amqp.name = new_state_name;
}
