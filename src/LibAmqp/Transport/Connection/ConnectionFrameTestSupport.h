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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_SOCKET_TEST_SUPPORT_H
#define LIBAMQP_TRANSPORT_CONNECTION_CONNECTION_SOCKET_TEST_SUPPORT_H

#include "Transport/LowLevel/ListenerThread.h"
#include "Transport/DummyBroker/EchoServer.h"
#include "Transport/Connection/Connection.h"

#include "Transport/Connection/ConnectionTestSupport.h"

namespace SuiteConnectionFrame
{
    class ConnectionFrameFixture : public SuiteConnection::BaseConnectionFixture
    {
    public:
        ConnectionFrameFixture();
        ~ConnectionFrameFixture();

        static void write_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_connection_action_f done_callback);
        static void read_intercept(amqp_connection_t *connection, amqp_buffer_t *buffer, size_t required, amqp_connection_read_callback_f done_callback);
        static void done_callback(amqp_connection_t *connection);

        static void set_test_data(test_data::TestData& data);
    public:
        amqp_buffer_t *buffer;

    private:
        static test_data::TestData *test_data_p;
        static amqp_buffer_t *write_copy;
    };
}
#endif
