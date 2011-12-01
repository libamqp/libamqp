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

#include "Transport/LowLevel/ListenerThread.h"
#include "Transport/DummyBroker/EchoServer.h"
#include "Transport/Connection/Connection.h"

#include "Transport/Connection/ConnectionSocketTestSupport.h"

#include "debug_helper.h"
SUITE(ConnectionWrite)
{
    class ConnectionWriteFixture : public SuiteConnectionSocket::ConnectionSocketFixture
    {
    public:
        ConnectionWriteFixture();
        ~ConnectionWriteFixture();

        int read_reply(size_t n);
        static void write_callback(amqp_connection_t *connection);

    public:
        unsigned char *reply;
    };

    ConnectionWriteFixture::ConnectionWriteFixture() : reply(0)
    {
        connect_to("localhost", 54321);
        loop_while_socket_state_is("Connecting");
    }

    ConnectionWriteFixture::~ConnectionWriteFixture()
    {
        free(reply);
    }

    void ConnectionWriteFixture::write_callback(amqp_connection_t *connection)
    {
    }

    int ConnectionWriteFixture::read_reply(size_t n)
    {
        if (reply) free(reply);
        reply = (unsigned char *) malloc(n);
        size_t needed = n;
        size_t so_far = 0;
        int c;
        while (needed > 0)
        {
            while ((c = read(connection->socket.fd, reply + so_far, needed)) != -1 && (needed > 0))
            {
                needed -= c;
                so_far += c;
            }
            if (c == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            {
                if (amqp_connection_writer_is_state(connection, "writing"))
                {
                    int ok = run_loop_with_timeout();
                    if (!ok)
                    {
                        return -1;
                    }
                }
            }
        }
        return c == -1 ? -1 : so_far;
    }

    TEST_FIXTURE(ConnectionWriteFixture, verify_fixture_cleans_resources)
    {
    }
    
    TEST_FIXTURE(ConnectionWriteFixture, stop_without_write)
    {
        amqp_connection_writer_initialize(connection);
        CHECK_EQUAL("initialized", connection->state.writer.name);
        connection->state.writer.enable(connection);
        CHECK_EQUAL("enabled", connection->state.writer.name);

        connection->state.writer.stop(connection, write_callback);
        CHECK_EQUAL("stopped", connection->state.writer.name);
    }

    TEST_FIXTURE(ConnectionWriteFixture, stop_without_prior_enable)
    {
        amqp_connection_writer_initialize(connection);
        CHECK_EQUAL("initialized", connection->state.writer.name);

        connection->state.writer.stop(connection, write_callback);
        CHECK_EQUAL("stopped", connection->state.writer.name);

        connection->state.writer.stop(connection, write_callback);
        CHECK_EQUAL("stopped", connection->state.writer.name);
    }

    TEST_FIXTURE(ConnectionWriteFixture, write_small_message)
    {
        amqp_connection_writer_initialize(connection);
        CHECK_EQUAL("initialized", connection->state.writer.name);
        connection->state.writer.enable(connection);
        CHECK_EQUAL("enabled", connection->state.writer.name);

        write_size(write_buffer, 1);
        amqp_buffer_putc(write_buffer, 'X');

        connection->state.writer.commence_write(connection, write_buffer, write_callback);
        read_reply(5);

        CHECK_EQUAL('X', reply[4]);
        connection->state.writer.stop(connection, write_callback);
        CHECK_EQUAL("stopped", connection->state.writer.name);
    }

    TEST_FIXTURE(ConnectionWriteFixture, write_huge_message)
    {
        amqp_connection_writer_initialize(connection);
        connection->state.writer.enable(connection);
        CHECK_EQUAL("enabled", connection->state.writer.name);

        const size_t size = 64 * 1024;
        amqp_buffer_grow(context, write_buffer, size);
        write_size(write_buffer, size - 4);
        for (size_t i = 4; i < size; i++)
        {
            amqp_buffer_putc(write_buffer, 19 + i % 67);
        }
        connection->state.writer.commence_write(connection, write_buffer, write_callback);

        read_reply(size);

        int failures = 0;
        for (size_t i = 4; i < size; i++)
        {
            int c = reply[i];
            int expect = 19 + i % 67;
            CHECK_EQUAL(expect, c);
            if (expect != c) failures++;
            if (failures > 5) break;
        }
        CHECK(failures < 5);
        connection->state.writer.stop(connection, write_callback);
        CHECK_EQUAL("stopped", connection->state.writer.name);
    }
}
