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

//#include "Context/Context.h"

#include "string.h"

#include "Context/Context.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Frame/Frame.h"
#include "Transport/Frame/FrameEncode.h"
#include "Codec/Encode/Encode.h"

#include "Transport/Amqp/AmqpFrames.h"

#include "debug_helper.h"


int amqp_amqp_prepare_open_frame(amqp_connection_t *connection)
{
    return amqp_encode_amqp_open(connection, connection->buffer.write);
}

void amqp_process_open_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    assert(frame->descriptor.id == 0x10);

not_implemented(todo);
}