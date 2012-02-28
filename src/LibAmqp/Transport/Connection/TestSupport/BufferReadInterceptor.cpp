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

#include <TestHarness.h>

#include "Transport/Connection/TestSupport/BufferReadInterceptor.h"
#include "TestData/TestFrames.h"

#include "debug_helper.h"

namespace TestSupport
{
    amqp_buffer_t *BufferReadInterceptor::saved_buffer = 0;

    void BufferReadInterceptor::set_data_for_read(amqp_buffer_t *buffer)
    {
        saved_buffer = buffer;
    }

    BufferReadInterceptor::BufferReadInterceptor()
    {
        connection->state.reader.commence_read = read_intercept;
    }

    BufferReadInterceptor::~BufferReadInterceptor()
    {
        connection->state.reader.commence_read = illegal_read_intercept;
    }

    void BufferReadInterceptor::illegal_read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f  read_callback)
    {
        amqp_fatal_program_error("Read intercept called after interceptor destroyed.");
    }

    void BufferReadInterceptor::read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f  read_callback)
    {
        assert(saved_buffer);
        if (connection->trace_flags & AMQP_TRACE_CONNECTION_READER)
        {
            amqp_context_printf(connection->context, 1, "intercepted read: ");
            amqp_dump_buffer(connection->context, saved_buffer, 0);
        }
        amqp_buffer_put_buffer_contents(buffer, saved_buffer);
        if (read_callback)
        {
            read_callback(connection, buffer, amqp_buffer_size(saved_buffer));
        }
        saved_buffer = 0;
    }
}
