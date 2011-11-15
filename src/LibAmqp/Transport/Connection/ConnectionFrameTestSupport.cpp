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

#include <TestHarness.h>

#include "Transport/Connection/ConnectionFrameTestSupport.h"
#include "TestData/TestFrames.h"

#include "debug_helper.h"

namespace SuiteConnectionFrame
{
    amqp_buffer_t *ConnectionFrameFixture::write_copy = 0;
    test_data::TestData **ConnectionFrameFixture::test_data_frames = 0;


    void ConnectionFrameFixture::set_test_data_for_read(test_data::TestData *frames[])
    {
        test_data_frames = frames;
    }

    void ConnectionFrameFixture::set_test_data_for_read(test_data::TestData& data)
    {
        static test_data::TestData *test_frames[] =
        {
            0,
            0
        };

        test_frames[0] = &data;
        set_test_data_for_read(test_frames);
    }

    ConnectionFrameFixture::ConnectionFrameFixture()
    {
        write_copy = amqp_allocate_buffer(context);
        buffer = amqp_allocate_buffer(context);

        connection = amqp_connection_create(context);
        connection->state.writer.commence_write = write_intercept;
        connection->state.reader.commence_read = read_intercept;
        connection->state.connection.done = done_callback;
    }

    ConnectionFrameFixture::~ConnectionFrameFixture()
    {
        amqp_connection_destroy(context, connection);
        amqp_deallocate_buffer(context, write_copy);
        amqp_deallocate_buffer(context, buffer);
    }

    void ConnectionFrameFixture::write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f write_callback)
    {
        amqp_buffer_reset(write_copy);
        amqp_buffer_put_buffer_contents(write_copy, buffer);
        if (connection->trace_flags & AMQP_TRACE_CONNECTION_WRITER)
        {
            amqp_context_printf(connection->context, 1, "written: ");
            amqp_dump_buffer(connection->context, buffer, 0);
        }
        write_callback(connection);
    }

    void ConnectionFrameFixture::read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f  read_callback)
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
    
    void ConnectionFrameFixture::done_callback(amqp_connection_t *connection)
    {
    }

    void ConnectionFrameFixture::done_callback(amqp_connection_t *connection, amqp_buffer_t *buffer)
    {
    }
}
