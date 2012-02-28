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

#include "Transport/Connection/TestSupport/WriteInterceptor.h"
#include "TestData/TestFrames.h"

#include "debug_helper.h"

namespace TestSupport
{
    amqp_buffer_t *WriteInterceptor::_intercepted_write = 0;

    WriteInterceptor::WriteInterceptor()
    {
        _intercepted_write = amqp_allocate_buffer(context);
        connection->state.writer.write = write_intercept;
    }

    WriteInterceptor::~WriteInterceptor()
    {
        connection->state.writer.write = illegal_write_intercept;
        amqp_deallocate_buffer(context, _intercepted_write);
    }

    amqp_buffer_t *WriteInterceptor::intercepted_write() const
    {
        return _intercepted_write;
    }

    void WriteInterceptor::illegal_write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer)
    {
        amqp_fatal_program_error("Write intercept called after interceptor destroyed.");
    }

    void WriteInterceptor::write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer)
    {
        amqp_buffer_reset(_intercepted_write);
        amqp_buffer_put_buffer_contents(_intercepted_write, buffer);
        if (connection->trace_flags & AMQP_TRACE_CONNECTION_WRITER)
        {
            amqp_context_printf(connection->context, 1, "intercepted write: ");
            amqp_dump_buffer(connection->context, buffer, 0);
        }
        amqp_deallocate_buffer(connection->context, buffer);
    }
}
