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

#include "Buffer/BufferTestSupport.h"
#include "Context/Context.h"

namespace t
{
    void amqp_buffer_dump_fragment(amqp_context_t* context, amqp_buffer_t *buffer, size_t start, size_t end)
    {
        size_t i, count;
        int old_indent;

        amqp_context_putc(context, '{');
        old_indent = amqp_context_increase_print_indent(context, 4);
        for (i = start, count = 0; i < end; i++, count++)
        {
            int c = buffer->bytes[i];
            if ((count & 0x0f) == 0)
            {
                amqp_context_putc(context, '\n');
            }

            amqp_context_putc(context, '0');
            amqp_context_putc(context, 'x');
            amqp_context_putc(context, to_hex(c >> 4));
            amqp_context_putc(context, to_hex(c & 0x0f));
            amqp_context_putc(context, ',');
            amqp_context_putc(context, ' ');
        }
        amqp_context_putc(context, '\n');
        amqp_context_set_print_indent(context, old_indent);
        amqp_context_putc(context, '}');
    }

    void amqp_buffer_dump(amqp_context_t* context, amqp_buffer_t *buffer)
    {
        amqp_buffer_dump_fragment(context, buffer, 0, buffer->limit.size);
    }

}


