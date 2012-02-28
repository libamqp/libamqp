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

#include "Transport/Connection/IO/ConnectionSocketTestFixture.h"

#include "debug_helper.h"

namespace SuiteConnectionSocket
{
    ConnectionSocketTestFixture::ConnectionSocketTestFixture()
    {
        bzero(message_buffer, sizeof(message_buffer));
    }

    ConnectionSocketTestFixture::~ConnectionSocketTestFixture()
    {
    }

    void ConnectionSocketTestFixture::write_size(amqp_buffer_t *buffer, uint32_t size)
    {
        amqp_four_byte_t value;
        value._uint = size;
        amqp_hton_32(buffer, value);
    }
}