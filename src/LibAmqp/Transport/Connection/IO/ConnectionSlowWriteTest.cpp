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

#include "Transport/LowLevel/ListenerThread.h"
#include "Transport/Connection/Connection.h"
#include "Context/TestSupport/BufferHolder.h"
#include "Transport/Connection/IO/ConnectionSocketTestFixture.h"
#include "Transport/LowLevel/Listener.h"
#include "Thread/Thread.h"

#include "debug_helper.h"
SUITE(SlowRunning)
{
    class ConnectionSlowWriteFixture :
            public virtual TestSupport::ClientConnectionHolder,
            public virtual TestSupport::ContextHolder,
            public virtual TestSupport::ReadBufferHolder
    {
    public:
        ConnectionSlowWriteFixture();
        ~ConnectionSlowWriteFixture();

        int write_bytes(int n);
        int fill_connection_buffers();
        void trigger_read();
        void signal_finished();

    private:
        void accept(amqp_context_t *context, int fd);
        static int accept_handler(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t address_size, amqp_accept_handler_arguments_t *accept_handler_arguments);

    private:
        amqp_accept_handler_arguments_t *accept_handler_arguments;
        amqp_listener_thread_t *listener_thread;
        amqp_mutex_t mutex;
        amqp_condition_variable_t cv;
        amqp_semaphore_t read_done_semaphore;
        int finished;
        int read_required;
        char bytes[512];
    };

    // runs in client thread
    void ConnectionSlowWriteFixture::signal_finished()
    {
        amqp_mutex_lock(&mutex);
        finished = true;
        amqp_condition_notify(&cv);
        amqp_mutex_unlock(&mutex);
    }

    // runs in client thread
    void ConnectionSlowWriteFixture::trigger_read()
    {
        amqp_mutex_lock(&mutex);
        read_required = true;
        amqp_condition_notify(&cv);
        amqp_mutex_unlock(&mutex);
        amqp_semaphore_wait(&read_done_semaphore);
    }

    // runs in listener thread
    void ConnectionSlowWriteFixture::accept(amqp_context_t *context, int fd)
    {
        char bytes[512];

        amqp_set_socket_to_nonblocking(fd);
        amqp_mutex_lock(&mutex);
        while (!finished)
        {
            amqp_condition_wait(&cv, &mutex);
            if (read_required)
            {
                int n;
                while ((n = read(fd, bytes, 512)) > 0)
                    ;

                read_required = false;
                amqp_semaphore_signal(&read_done_semaphore);
            }
        }
        amqp_mutex_unlock(&mutex);
        close(fd);
    }

    int ConnectionSlowWriteFixture::accept_handler(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t address_size, amqp_accept_handler_arguments_t *accept_handler_arguments)
    {
        ConnectionSlowWriteFixture *fixture = (ConnectionSlowWriteFixture *) accept_handler_arguments->user_data;
        fixture->accept(me->context, fd);
        return 0;
    }

    ConnectionSlowWriteFixture::ConnectionSlowWriteFixture() : finished(0), read_required(0)
    {
        bzero(bytes, sizeof(bytes));

        amqp_mutex_initialize(&mutex);
        amqp_condition_initialize(&cv);
        amqp_semaphore_initialize(&read_done_semaphore);

        amqp_event_loop_t *default_event_loop = 0;
        accept_handler_arguments = amqp_accept_handler_arguments_initialize(context, 0, this);

        listener_thread = amqp_listener_thread_initialize(context, default_event_loop, TestParameters::port, accept_handler, "listener", accept_handler_arguments);
    }

    ConnectionSlowWriteFixture::~ConnectionSlowWriteFixture()
    {
        signal_finished();

        amqp_listener_thread_destroy(context, listener_thread);
        amqp_accept_handler_arguments_destroy(context, accept_handler_arguments);
        amqp_semaphore_destroy(&read_done_semaphore);
        amqp_condition_destroy(&cv);
        amqp_mutex_destroy(&mutex);
    }

    int ConnectionSlowWriteFixture::write_bytes(int n)
    {
        return write(connection->socket.fd, bytes, n);
    }

    int ConnectionSlowWriteFixture::fill_connection_buffers()
    {
        int n;
        int sum = 0;
        while ((n = write_bytes(512)) > 0)
        {
            sum += n;
        }
        return sum;
    }

    static void writer_stop_callback(amqp_connection_t *connection)
    {
    }

    TEST_FIXTURE(ConnectionSlowWriteFixture, read_is_delayed_if_send_buffers_full)
    {
        connect_to("localhost", TestParameters::port);
        loop_until_connection_socket_state_is("connected");
        fill_connection_buffers();

        int n = write_bytes(512);
        CHECK_EQUAL(-1, n);

        CHECK_EQUAL("initialized", connection->state.writer.name);
        connection->state.writer.enable(connection);
        CHECK_EQUAL("enabled", connection->state.writer.name);

        amqp_buffer_t *buffer = amqp_allocate_buffer(context);
        for (int i = 0; i < (int) amqp_buffer_capacity(buffer); i++)
        {
            amqp_buffer_putc(buffer, i % 256);
        }

        connection->state.writer.write(connection, buffer);
        CHECK_EQUAL("writing", connection->state.writer.name);

        run_loop_with_timeout(0.1);
        CHECK_EQUAL("writing", connection->state.writer.name);

        trigger_read();

        CHECK_EQUAL("writing", connection->state.writer.name);
        run_loop_with_timeout(0.1);
        CHECK_EQUAL("enabled", connection->state.writer.name);

        connection->state.writer.stop(connection, writer_stop_callback);
    }

    TEST_FIXTURE(ConnectionSlowWriteFixture, delayed_read_fails_when_connection_closed)
    {
        connect_to("localhost", TestParameters::port);
        loop_until_connection_socket_state_is("connected");
        fill_connection_buffers();

        int n = write_bytes(512);
        CHECK_EQUAL(-1, n);

        CHECK_EQUAL("initialized", connection->state.writer.name);
        connection->state.writer.enable(connection);
        CHECK_EQUAL("enabled", connection->state.writer.name);

        amqp_buffer_t *buffer = amqp_allocate_buffer(context);
        for (int i = 0; i < (int) amqp_buffer_capacity(buffer); i++)
        {
            amqp_buffer_putc(buffer, i % 256);
        }

        connection->state.writer.write(connection, buffer);
        CHECK_EQUAL("writing", connection->state.writer.name);

        signal_finished();

        CHECK_EQUAL("writing", connection->state.writer.name);
        run_loop_with_timeout(0.1);
        CHECK_EQUAL("failed", connection->state.writer.name);

        connection->state.writer.stop(connection, writer_stop_callback);
    }
}