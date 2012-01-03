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
#include "Transport/Connection/FrameDispatch.h"
#include "Transport/Connection/ConnectionState.h"

#include "debug_helper.h"

void amqp_dispatch_sasl_mechanisms(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.sasl.frame.mechanisms;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_sasl_init(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.sasl.frame.init;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_sasl_challenge(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.sasl.frame.challenge;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_sasl_response(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.sasl.frame.response;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_sasl_outcome(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.sasl.frame.outcome;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_amqp_open(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.amqp.frame.open;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_amqp_begin(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.amqp.frame.begin;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_amqp_attach(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.amqp.frame.attach;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_amqp_flow(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.amqp.frame.flow;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_amqp_transfer(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.amqp.frame.transfer;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_amqp_disposition(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.amqp.frame.disposition;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_amqp_detach(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.amqp.frame.detach;
    assert(dispatch);
    dispatch(connection, frame);
}

void amqp_dispatch_amqp_end(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.amqp.frame.end;
    assert(dispatch);
    dispatch(connection, frame);
}


void amqp_dispatch_amqp_close(amqp_connection_t *connection, amqp_frame_t *frame)
{
    amqp_message_dispatch_t dispatch = connection->state.amqp.frame.close;
    assert(dispatch);
    dispatch(connection, frame);
}
