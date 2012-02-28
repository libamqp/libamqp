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

//#include "Context/Context.h"

#include "string.h"

#include "Context/Context.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Frame/Frame.h"
#include "Transport/Frame/FrameEncode.h"
#include "Codec/Encode/Encode.h"

#include "Transport/Amqp/AmqpFrames.h"
#include "AmqpTypes/AmqpError.h"

#include "debug_helper.h"

int amqp_send_amqp_open_frame(amqp_connection_t *connection)
{
    amqp_buffer_t *buffer = amqp_encode_amqp_open(connection);
    amqp_connection_write_buffer(connection, buffer);
    return buffer != 0;
}

int amqp_process_open_frame_from_client(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_transport_open_t *open_request;

    assert(!amqp_connection_is_client(connection));
    assert(frame && frame->descriptor.id == 0x10);

    open_request = &frame->frames.amqp.open;
    connection->amqp.connection.remote_container_id = amqp_string_to_cstr(connection->context, &open_request->container_id);

    if (amqp_string_is_null(&open_request->hostname))
    {
        // TODO - if hostname from client is null, use the hostname that arrived in SASL or TLS handshake
        amqp_frame_dump(connection->context, frame);
        not_implemented(todo);
    }

    connection->amqp.connection.hostname = amqp_string_to_cstr(connection->context, &open_request->hostname);

    connection->amqp.connection.remote_limits.max_frame_size = open_request->max_frame_size;
    connection->amqp.connection.remote_limits.channel_max = open_request->channel_max;
    connection->amqp.connection.remote_limits.idle_time_out = open_request->idle_time_out;

    return true;
}

int amqp_process_open_frame_from_broker(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_transport_open_t *open_request;

    assert(amqp_connection_is_client(connection));
    assert(frame && frame->descriptor.id == 0x10);

    open_request = &frame->frames.amqp.open;
    connection->amqp.connection.remote_container_id = amqp_string_to_cstr(connection->context, &open_request->container_id);

    if (!amqp_string_is_null(&open_request->hostname))
    {
        connection->amqp.connection.hostname = amqp_string_to_cstr(connection->context, &open_request->hostname);
    }

    connection->amqp.connection.remote_limits.max_frame_size = open_request->max_frame_size;
    connection->amqp.connection.remote_limits.channel_max = open_request->channel_max;
    connection->amqp.connection.remote_limits.idle_time_out = open_request->idle_time_out;

    return true;
}

int amqp_send_amqp_close_frame(amqp_connection_t *connection)
{
    amqp_buffer_t *buffer = amqp_encode_amqp_close(connection);
    amqp_connection_write_buffer(connection, buffer);
    return buffer != 0;
}

int amqp_process_close_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_transport_close_t *close;

    assert(frame && frame->descriptor.id == 0x18);

    close = &frame->frames.amqp.close;

    if (amqp_error_is_defined(&close->error))
    {
        not_implemented(todo);
        // TODO - store error detail in the connection
    }

    return 1;
}

int amqp_send_empty_frame(amqp_connection_t *connection)
{
    amqp_buffer_t *buffer = amqp_encode_empty_frame(connection);
    amqp_connection_write_buffer(connection, buffer);
    return buffer != 0;
}

int amqp_send_amqp_begin_frame(amqp_connection_t *connection, amqp_session_t *session)
{
    amqp_buffer_t *buffer = amqp_encode_amqp_begin(connection, session);
    amqp_connection_write_buffer(connection, buffer);
    return buffer != 0;
}
int amqp_process_begin_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_transport_begin_t *begin_request;

    assert(amqp_connection_is_client(connection));
    assert(frame && frame->descriptor.id == 0x11);

    begin_request = &frame->frames.amqp.begin;

//    if (amqp_frame_field_is_null(frame, 0))
//    {
//        SOUTS("start sesion request frame");
//    }
//    else
//    {
//        SOUTS("start sesion reply frame");
//    }

//    begin_request->
//    connection->amqp.connection.remote_container_id = amqp_string_to_cstr(connection->context, &begin_request->container_id);
//
//    if (!amqp_string_is_null(&open_request->hostname))
//    {
//        connection->amqp.connection.hostname = amqp_string_to_cstr(connection->context, &open_request->hostname);
//    }
//
//    connection->amqp.connection.remote_limits.max_frame_size = open_request->max_frame_size;
//    connection->amqp.connection.remote_limits.channel_max = open_request->channel_max;
//    connection->amqp.connection.remote_limits.idle_time_out = open_request->idle_time_out;
//    not_implemented(todo);
    return true;
}

