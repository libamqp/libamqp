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

#include <string.h>

#include "Buffer/Buffer.h"
#include "Memory/Memory.h"

#include "Context/ErrorHandling.h"

static
void amqp_initialize_amqp_buffer_t(amqp_context_t *context, amqp_memory_pool_t *pool, amqp_buffer_t *buffer)
{
    const int size = 1024 * 4;
    buffer->bytes = amqp_malloc(context, size);

    buffer->actual_size = size;
    buffer->limit.index = 0;
    buffer->limit.size = 0;
}

static
void amqp_cleanup_amqp_buffer_t(amqp_context_t *context, amqp_memory_pool_t *pool, amqp_buffer_t *buffer)
{
    if (buffer)
    {
        amqp_free(context, buffer->bytes);
    }
}

void amqp_buffer_initialize_pool(amqp_memory_pool_t *pool)
{
    amqp_initialize_pool_suggesting_block_size(pool, sizeof(amqp_buffer_t), 512);
    amqp_pool_specify_initialization_callbacks(pool, (amqp_pool_callback_t) amqp_initialize_amqp_buffer_t, (amqp_pool_callback_t) amqp_cleanup_amqp_buffer_t);
}

int amqp_buffer_putc(amqp_buffer_t *buffer, int c)
{
    if (buffer->limit.size >= buffer->actual_size)
    {
        return -1;
    }

    return buffer->bytes[buffer->limit.size++] = c;
}

int amqp_buffer_puts(amqp_buffer_t *buffer, const unsigned char *source, size_t n)
{
    if ((buffer->limit.size + n) >= buffer->actual_size)
    {
        return 0;
    }

    memcpy(buffer->bytes + buffer->limit.size, source, n);
    buffer->limit.size += n;

    return n;
}

// check on available space is relative to the current limit.
int amqp_buffer_check_free_space(amqp_buffer_t *buffer, size_t needed)
{
    if ((buffer->limit.size + needed) > buffer->actual_size)
    {
        return 0;
    }
    return 1;
}

void amqp_buffer_backup(amqp_buffer_t *buffer, size_t amount)
{
    assert(buffer->limit.size >= amount);
    buffer->limit.size -= amount;
}

/*
    TODO - move into test file
    Used by test code, so don't worry about buffer chaining
*/
int amqp_buffer_put_buffer_contents(amqp_buffer_t *dest, amqp_buffer_t *source)
{
    assert(source->limit.size <= dest->actual_size);
    return amqp_buffer_puts(dest, source->bytes, source->limit.size);
}

uint32_t amqp_buffer_read_size(amqp_buffer_t *buffer, size_t width)
{
    uint32_t result;

    switch (width)
    {
    case 4:
        if (!amqp_buffer_check_available(buffer, width))
        {
            return -1;
        }
        result = amqp_buffer_unchecked_getc(buffer);
        result = (result << 8) | amqp_buffer_unchecked_getc(buffer);
        result = (result << 8) | amqp_buffer_unchecked_getc(buffer);
        result = (result << 8) | amqp_buffer_unchecked_getc(buffer);
        break;

    case 1:
        result = amqp_buffer_getc(buffer);
        break;

    default:
        amqp_fatal_program_error("Asked to read an invalid size field.");
        return 0;
    }

    return result;
}
