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

#include "Transport/Connection/TestSupport/ReadInterceptor.h"
#include "TestData/TestFrames.h"

#include "debug_helper.h"

namespace TestSupport
{
    test_data::TestData **ReadInterceptor::test_data_frames = 0;

    void ReadInterceptor::set_test_data_for_read(test_data::TestData *frames[])
    {
        test_data_frames = frames;
    }

    void ReadInterceptor::set_test_data_for_read(test_data::TestData& data)
    {
        static test_data::TestData *test_frames[] =
        {
            0,
            0
        };

        test_frames[0] = &data;
        set_test_data_for_read(test_frames);
    }

    ReadInterceptor::ReadInterceptor()
    {
        connection->state.reader.commence_read = read_intercept;
    }

    ReadInterceptor::~ReadInterceptor()
    {
        connection->state.reader.commence_read = illegal_read_intercept;
    }

    void ReadInterceptor::illegal_read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f  read_callback)
    {
        amqp_fatal_program_error("Read intercept called after interceptor destroyed.");
    }

    void ReadInterceptor::read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f  read_callback)
    {
//        assert(*test_data_frames != 0);
        if (test_data_frames && *test_data_frames)
        {
            assert(buffer != 0);

            size_t index = amqp_buffer_index(buffer);
            size_t size = amqp_buffer_size(buffer) + required;

            amqp_buffer_reset(buffer);
            (*test_data_frames)->transfer_to(buffer);

            // Move on to the next test frame if the current one has been read.
            if (size == amqp_buffer_size(buffer))
            {
                test_data_frames++;
            }

            amqp_buffer_advance_read_index(buffer, index);
            amqp_buffer_set_write_point(buffer, size);

            if (connection->trace_flags & AMQP_TRACE_CONNECTION_READER)
            {
                amqp_context_printf(connection->context, 1, "read: ");
                amqp_dump_buffer(connection->context, buffer, 0);
            }

            read_callback(connection, buffer, required);
        }
    }
}
