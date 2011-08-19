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
#include "Transport/Transport.h"

#include "Context/Context.h"
#include "Transport/EventThreadTestSupport.h"
#include "Transport/Listener.h"
#include "Transport/Connect.h"
#include "Transport/DummyBroker/DummyBroker.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "debug_helper.h"

SUITE(Transport)
{
    class ListenerFixture : public EventThreadFixture
    {
    public:
        ListenerFixture() : EventThreadFixture()
        {
            m_event_thread = ListenerFixture::thread_create(context);
        }
        ~ListenerFixture() { }
        int connect(struct sockaddr *address, socklen_t address_size);
        void client_send_and_receive(int fd);

    public:
        static const int port_number;

        static amqp_event_thread_t *thread_create(amqp_context_t *context);
        static void listen_thread_handler(amqp_event_thread_t *event_thread);
        static int accept_new_connection(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t adress_size);
    };

    const int ListenerFixture::port_number = 7654;

    amqp_event_thread_t *ListenerFixture::thread_create(amqp_context_t *context)
    {
        struct ev_loop *loop = ev_default_loop(0);
        return amqp_event_thread_initialize(context, ListenerFixture::listen_thread_handler, loop, 0);
    }

    int ListenerFixture::accept_new_connection(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t adress_size)
    {
        char buffer[128];
        int n;

        set_socket_to_blocking(fd);

        while ((n = read(fd, buffer, sizeof(buffer) -1)) > 0)
        {
            write (fd, buffer, n);
        }
        close(fd);
        return 0;
    }

    void ListenerFixture::listen_thread_handler(amqp_event_thread_t *event_thread)
    {
        amqp_io_event_watcher_t *accept_watcher = amqp_listener_initialize(event_thread->context, event_thread->loop, ListenerFixture::port_number, ListenerFixture::accept_new_connection);

        amqp_event_thread_run_loop(event_thread);

        amqp_listener_destroy(event_thread->context, accept_watcher);
    }

    int ListenerFixture::connect(struct sockaddr *address, socklen_t address_size)
    {
        int socket_fd = socket(address->sa_family, SOCK_STREAM, 0);
        CHECK(socket_fd != -1);

        int rc = ::connect(socket_fd, address, address_size);
        CHECK(rc != -1);

        return socket_fd;
    }

    void ListenerFixture::client_send_and_receive(int socket_fd)
    {
        const char *message = "Hello";
        write(socket_fd, message, strlen(message));

        char buffer[128];
        bzero(buffer, sizeof(buffer));
        int n = read(socket_fd, buffer, sizeof(buffer) - 1);
        buffer[n > 0 ? n : 0] = '\0';

        CHECK_EQUAL(message, buffer);

        int rc = close(socket_fd);
        CHECK(rc != -1);
    }

    TEST_FIXTURE(ListenerFixture, connect_to_listener_with_IPv4)
    {
        struct sockaddr_in address;
        bzero(&address, sizeof(address));

        address.sin_family = AF_INET;
        address.sin_port = htons(port_number);
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        int fd = ListenerFixture::connect((struct sockaddr *) &address, sizeof(address));
        ListenerFixture::client_send_and_receive(fd);
    }

    TEST_FIXTURE(ListenerFixture, connect_to_listener_with_IPv6)
    {
        struct sockaddr_in6 address;
        bzero(&address, sizeof(address));

        address.sin6_family = AF_INET6;
        address.sin6_flowinfo = 0;
        address.sin6_port = htons(port_number);
        address.sin6_addr = in6addr_loopback;

        int fd = ListenerFixture::connect((struct sockaddr *) &address, sizeof(address));
        ListenerFixture::client_send_and_receive(fd);
    }

    TEST_FIXTURE(ListenerFixture, connect_to_listener_using_lookup)
    {
        int fd = amqp_blocking_tcp_connect_to("localhost", port_number);
        ListenerFixture::client_send_and_receive(fd);
    }

    TEST(use_dummy_broker_to_listen)
    {
        amqp_context_t *context = amqp_create_context();
        amqp_dummy_broker_t *broker = amqp_dummy_broker_initialize(context, ListenerFixture::port_number, ListenerFixture::accept_new_connection);

        int fd = amqp_blocking_tcp_connect_to("localhost", ListenerFixture::port_number);

        const char *message = "Hello there";
        write(fd, message, strlen(message));

        char buffer[128];
        bzero(buffer, sizeof(buffer));
        int n = read(fd, buffer, sizeof(buffer) - 1);
        buffer[n > 0 ? n : 0] = '\0';

        CHECK_EQUAL(message, buffer);

        int rc = close(fd);
        CHECK(rc != -1);

        amqp_dummy_broker_destroy(context, broker);
        amqp_context_destroy(context);
    }
}
