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
#include "Transport/TransportTestSupport.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

SUITE(Transport)
{
    class ListenerFixture : public TransportFixture
    {
    public:
        ListenerFixture()
        {
        }

        ~ListenerFixture()
        {
        }

    static const int port_number;
    private:
    };

    const int ListenerFixture::port_number = 7654;

    TEST_FIXTURE(ListenerFixture, connect_to_listener_with_IPv4)
    {
        struct sockaddr_in address;
        bzero(&address, sizeof(address));

        address.sin_family = AF_INET;
        address.sin_port = htons(port_number);
        address.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

        int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        CHECK(socket_fd != -1);

        int rc = connect(socket_fd, (struct sockaddr *) &address, sizeof(address));
        CHECK(rc != -1);

        char buffer[128];
        bzero(buffer, sizeof(buffer));
        read(socket_fd, buffer, sizeof(buffer) - 1);
        CHECK_EQUAL("Hello", buffer);

        rc = close(socket_fd);
        CHECK(rc != -1);
    }

    TEST_FIXTURE(ListenerFixture, connect_to_listener_with_IPv6)
    {
        struct sockaddr_in6 address;
        bzero(&address, sizeof(address));

        address.sin6_family = AF_INET6;
        address.sin6_flowinfo = 0;
        address.sin6_port = htons(port_number);
        address.sin6_addr = in6addr_loopback;

        int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
        CHECK(socket_fd != -1);

        int rc = connect(socket_fd, (struct sockaddr *) &address, sizeof(address));
        CHECK(rc != -1);

        char buffer[128];
        bzero(buffer, sizeof(buffer));
        read(socket_fd, buffer, sizeof(buffer) - 1);
        CHECK_EQUAL("Hello", buffer);

        rc = close(socket_fd);
        CHECK(rc != -1);
    }
}
