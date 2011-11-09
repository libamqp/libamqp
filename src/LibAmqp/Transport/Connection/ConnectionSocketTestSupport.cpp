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

#include "Transport/Connection/ConnectionSocketTestSupport.h"

#include "debug_helper.h"

namespace SuiteConnectionSocket
{
    ConnectionSocketFixture::ConnectionSocketFixture()
    {
        connection = amqp_connection_create(context);

        write_buffer = amqp_allocate_buffer(context);
        read_buffer = amqp_allocate_buffer(context);
        echo_server_thread = amqp_echo_server_initialize(context, 54321);
        bzero(message_buffer, sizeof(message_buffer));
    }

    ConnectionSocketFixture::~ConnectionSocketFixture()
    {
        amqp_deallocate_buffer(context, read_buffer);
        amqp_deallocate_buffer(context, write_buffer);
        amqp_connection_destroy(context, connection);
        amqp_echo_server_destroy(context, echo_server_thread);
    }
}