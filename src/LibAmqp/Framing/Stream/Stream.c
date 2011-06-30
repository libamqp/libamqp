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

#include <unistd.h>

#include "Context/Context.h"
#include "Framing/Stream/Stream.h"

static void amqp_buffer_stream_destroy(amqp_context_t *context, amqp_stream_t *stream);
static int amqp_buffer_stream_read(amqp_stream_t *stream, unsigned char *buffer, size_t bufsiz, size_t offset, int n);
static int amqp_buffer_stream_write(amqp_stream_t *stream, unsigned char *buffer, int n);

static void amqp_socket_stream_destroy(amqp_context_t *context, amqp_stream_t *stream);
static int amqp_socket_stream_read(amqp_stream_t *stream, unsigned char *buffer, size_t bufsiz, size_t offset, int n);
static int amqp_socket_stream_write(amqp_stream_t *stream, unsigned char *buffer, int n);

struct amqp_stream_t
{
    void (*destroy)(amqp_context_t *context, amqp_stream_t *stream);
    int (*read)(amqp_stream_t *stream, unsigned char *buffer, size_t bufsiz, size_t offset, int n);
    int (*write)(amqp_stream_t *stream, unsigned char *buffer, int n);

    amqp_buffer_t *buffer;
    int fd;
};

void amqp_destroy_stream(amqp_context_t *context, amqp_stream_t *stream)
{
    if (stream)
    {
        (*stream->destroy)(context, stream);
    }
}

int amqp_stream_read(amqp_stream_t *stream, unsigned char *buffer, size_t bufsiz, size_t offset, int n)
{
    assert(stream != 0);
    return (*stream->read)(stream, buffer, bufsiz, offset, n);
}

int amqp_stream_write(amqp_stream_t *stream, unsigned char *buffer, int n)
{
    assert(stream != 0);
    return (*stream->write)(stream, buffer, n);
}

amqp_stream_t *amqp_buffer_stream_create(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_stream_t *result = AMQP_MALLOC(amqp_stream_t);
    result->destroy = amqp_buffer_stream_destroy;
    result->read = amqp_buffer_stream_read;
    result->write = amqp_buffer_stream_write;

    result->buffer = amqp_allocate_buffer(context);
    amqp_buffer_put_buffer_contents(result->buffer, buffer);

    return result;
}

static void amqp_buffer_stream_destroy(amqp_context_t *context, amqp_stream_t *stream)
{
    amqp_deallocate_buffer(context, stream->buffer);
    amqp_free(stream);
}

static  int amqp_buffer_stream_read(amqp_stream_t *stream, unsigned char *buffer, size_t bufsiz, size_t offset, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        int c;
        if ((offset + i) >= bufsiz)
        {
            return i;
        }
        if ((c = amqp_buffer_getc(stream->buffer)) == -1)
        {
            return i;
        }
        buffer[offset + i] = c;
    }
    return n;
}

static  int amqp_buffer_stream_write(amqp_stream_t *stream, unsigned char *buffer, int n)
{
    abort();
}

amqp_stream_t *amqp_socket_stream_create(amqp_context_t *context, int fd)
{
    amqp_stream_t *result = AMQP_MALLOC(amqp_stream_t);
    result->destroy = amqp_socket_stream_destroy;
    result->read = amqp_socket_stream_read;
    result->write = amqp_socket_stream_write;

    result->fd = fd;
    return result;
}

static void amqp_socket_stream_destroy(amqp_context_t *context, amqp_stream_t *stream)
{
    if (stream->fd != -1)
    {
        close(stream->fd);
        stream->fd = -1;
    }
    amqp_free(stream);
}

static  int amqp_socket_stream_read(amqp_stream_t *stream, unsigned char *bytes, size_t bufsiz, size_t offset, int n)
{
    int space = bufsiz - offset;

    if (space <= 0)
    {
        abort();
    }

    return read(stream->fd, bytes + offset, n > space ? space : n);
}

static  int amqp_socket_stream_write(amqp_stream_t *stream, unsigned char *bytes, int n)
{
    return write(stream->fd, bytes, n);
}
