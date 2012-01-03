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

#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#endif

#include "Context/Context.h"
#include "Transport/DummyBroker/EchoServer.h"

#include "debug_helper.h"

static int read_all(int fd, amqp_buffer_t *buffer, int wanted)
{
    int count = wanted;
    char block[128];
    do
    {
        // Small block read size is deliberate. Want writer to backup.
        int n = read(fd, block, wanted < 128 ? wanted : 128);
        if (n == -1)
        {
            if (errno != EAGAIN && errno != EINTR)
            {
                return -1;
            }
        }
        else if (n == 0)
        {
            return 0;
        }
        else
        {
            int i;
            for (i = 0; i < n; i++)
            {
                amqp_buffer_putc(buffer, block[i]);
            }
            count -= n;
        }
    } while (count > 0);

    return wanted;
}

static int write_all(int fd, amqp_buffer_t *buffer)
{
    int wanted = amqp_buffer_size(buffer);
    int count = wanted;

    while (count > 0)
    {
        struct iovec *iovec;
        int iov_count;
        int written;

        iovec = amqp_buffer_read_io_vec(buffer, &iov_count);
        written = writev(fd, iovec, iov_count);
        if (written == -1)
        {
            if (errno != EAGAIN && errno != EINTR)
            {
                return -1;
            }
        }
        else
        {
            count -= written;
            amqp_buffer_advance_read_index(buffer, written);
        }
    }

    return wanted;
}

static int echo(amqp_context_t *context, int fd, amqp_buffer_t *buffer)
{
    const int max_size = 64 * 1024 - 4;
    int n;
    uint32_t size;
    amqp_set_socket_to_blocking(fd);

    if ((n = read_all(fd, buffer, 4)) <= 0)
    {
        return n;
    }
    size = amqp_buffer_read_size_field(buffer, 4);
    if (size > max_size)
    {
        return -1;
    }
    amqp_buffer_grow(context, buffer, size + 4);

    if ((n = read_all(fd, buffer, size)) <= 0)
    {
        return n;
    }

    // Want to write everything, including leading 4 bytes
    amqp_buffer_reset_index(buffer);

    if (write_all(fd, buffer) == -1)
    {
        return -1;
    }

    return 0;
}

static int echo_server_new_connection_handler(amqp_io_event_watcher_t *me, amqp_event_loop_t *loop, int fd, struct sockaddr_storage *client_address, socklen_t address_size, amqp_accept_handler_arguments_t *accept_handler_arguments)
{
    amqp_context_t *context = me->context;

    amqp_buffer_t *buffer = amqp_allocate_buffer(context);
    amqp_set_socket_to_blocking(fd);

    echo(context, fd, buffer);

    close(fd);
    amqp_deallocate_buffer(context, buffer);
    return 0;
}

amqp_listener_thread_t *amqp_echo_server_initialize(amqp_context_t *context, int listen_port_number)
{
    amqp_event_loop_t *default_event_loop = 0;
    return amqp_listener_thread_initialize(context, default_event_loop, listen_port_number, echo_server_new_connection_handler, "echo-server", 0);
}

void amqp_echo_server_destroy(amqp_context_t *context, amqp_listener_thread_t *echo_server_thread)
{
    amqp_listener_thread_destroy(context, echo_server_thread);
}
