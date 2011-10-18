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

    test_data::TestData *ConnectionFrameFixture::test_data_p = 0;

    ConnectionFrameFixture::ConnectionFrameFixture()
    {
        buffer = amqp_allocate_buffer(context);

        connection = amqp_connection_initialize(context);
        connection->state.writer.commence_write = write_intercept;
        connection->state.reader.commence_read = read_intercept;
    }

    ConnectionFrameFixture::~ConnectionFrameFixture()
    {
        amqp_connection_destroy(context, connection);
        amqp_deallocate_buffer(context, buffer);
    }

    void ConnectionFrameFixture::write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f write_callback)
    {
//        SOUTS("write_intercept called");
        not_implemented(todo);
    }
    void ConnectionFrameFixture::read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f  read_callback)
    {
//        SOUTS("read_intercept called");
//        SOUTV(required);
        if (test_data_p)
        {
            assert(buffer != 0);

            size_t index = amqp_buffer_index(buffer);
            size_t size = amqp_buffer_size(buffer) + required;
            amqp_buffer_reset(buffer);
            test_data_p->transfer_to(buffer);
            amqp_buffer_advance_read_index(buffer, index);
            amqp_buffer_set_write_point(buffer, size);

            read_callback(connection, buffer, required);
        }
    }
    
    void ConnectionFrameFixture::done_callback(amqp_connection_t *connection)
    {
    }
}
