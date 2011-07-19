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

#include <string.h>

#include "Context/Context.h"
#include "Framing/Stream/Stream.h"


int amqp_buffer_read_from_stream(amqp_buffer_t *buffer, amqp_stream_t *stream, int n)
{
    assert(buffer != 0);
    assert(stream != 0);

    n = amqp_stream_read(stream, buffer->bytes, buffer->actual_size, buffer->limit.size, n);
    if (n > 0)
    {
        buffer->limit.size += n;
    }
    return n;
}

int amqp_buffer_write_to_stream(amqp_buffer_t *buffer, amqp_stream_t *stream)
{
    assert(buffer != 0);
    assert(stream != 0);

    return amqp_stream_write(stream, buffer->bytes, buffer->limit.size);
}
