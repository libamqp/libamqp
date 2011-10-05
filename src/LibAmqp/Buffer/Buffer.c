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

#include <assert.h>
#include <string.h>

#include "Buffer/Buffer.h"
#include "Buffer/BufferInternal.h"
#include "Memory/Memory.h"
#include "Misc/Bits.h"

size_t amqp_buffer_grow(amqp_context_t *context, amqp_buffer_t *buffer, size_t new_size)
{
    if (new_size > AMQP_BUFFER_MAX_SIZE)
    {
        new_size = AMQP_BUFFER_MAX_SIZE;
    }
    if (new_size > buffer->capacity)
    {
        size_t needed_blocks = (new_size - 1 ) / AMQP_BUFFER_FRAGMENT_SIZE + 1;
        size_t block;

        for (block = buffer->capacity / AMQP_BUFFER_FRAGMENT_SIZE; block < needed_blocks; block++)
        {
            buffer->fragments[block] = amqp_malloc(context, AMQP_BUFFER_FRAGMENT_SIZE);
        }

        buffer->capacity = needed_blocks * AMQP_BUFFER_FRAGMENT_SIZE;
    }
    return buffer->capacity;
}

static
void amqp_initialize_amqp_buffer_t(amqp_context_t *context, amqp_memory_pool_t *pool, amqp_buffer_t *buffer)
{
    buffer->fragments[0] = amqp_malloc(context, AMQP_BUFFER_FRAGMENT_SIZE);

    buffer->capacity = AMQP_BUFFER_FRAGMENT_SIZE;
    buffer->limit.index = 0;
    buffer->limit.size = 0;
}

static
void amqp_cleanup_amqp_buffer_t(amqp_context_t *context, amqp_memory_pool_t *pool, amqp_buffer_t *buffer)
{
    if (buffer)
    {
        size_t actual_blocks = buffer->capacity / AMQP_BUFFER_FRAGMENT_SIZE;
        size_t block;

        for (block = 0; block < actual_blocks; block++)
        {
            amqp_free(context, buffer->fragments[block]);
        }
    }
}

void amqp_buffer_initialize_pool(amqp_memory_pool_t *pool)
{
    assert(amqp_is_power_of_two(AMQP_BUFFER_MAX_SIZE));
    assert(amqp_is_power_of_two(AMQP_BUFFER_FRAGMENT_SIZE));
    assert(AMQP_FRAGMENT_INDEX_BITS == amqp_trailing_zeros(AMQP_BUFFER_FRAGMENT_SIZE));

    amqp_initialize_pool_suggesting_block_size(pool, sizeof(amqp_buffer_t), 1024);
    amqp_pool_specify_initialization_callbacks(pool, (amqp_pool_callback_t) amqp_initialize_amqp_buffer_t, (amqp_pool_callback_t) amqp_cleanup_amqp_buffer_t);
}

int amqp_buffer_puts(amqp_buffer_t *buffer, const unsigned char *source, size_t n)
{
    size_t i;
    if ((buffer->limit.size + n) >= buffer->capacity)
    {
        return 0;
    }
    for (i = 0; i < n; i++)
    {
        amqp_buffer_putc(buffer, source[i]);
    }
    return n;
}

void amqp_buffer_backup(amqp_buffer_t *buffer, size_t amount)
{
    assert(buffer->limit.size >= amount);
    buffer->limit.size -= amount;
}

/*
    TODO - move into test file
*/
int amqp_buffer_put_buffer_contents(amqp_buffer_t *dest, amqp_buffer_t *source)
{
    size_t i;

    assert(source->limit.size <= dest->capacity);

    for (i = 0; i < source->limit.size; i++)
    {
        amqp_unchecked_putc(dest, amqp_unchecked_getc_at(source, i));
    }
    return source->limit.size;
}

uint32_t amqp_buffer_read_size_field(amqp_buffer_t *buffer, size_t width)
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
        abort();
        return 0;
    }

    return result;
}

static int amqp_buffer_fill_io_vec(amqp_buffer_t *buffer, size_t n, size_t block, size_t offset)
{
    size_t available = AMQP_BUFFER_FRAGMENT_SIZE - offset;
    int i = 0;

    buffer->io_vectors[0].iov_base = &buffer->fragments[block][offset];

    if (n <= available)
    {
        buffer->io_vectors[i++].iov_len = n;
    }
    else
    {
        buffer->io_vectors[i++].iov_len = available;
        n -= available;

        while (n > 0)
        {
            block++;
            buffer->io_vectors[i].iov_base = buffer->fragments[block];
            available =  n > AMQP_BUFFER_FRAGMENT_SIZE ? AMQP_BUFFER_FRAGMENT_SIZE : n;
            buffer->io_vectors[i++].iov_len = available;
            n -= available;
        }
    }
    return i;
}

struct iovec *amqp_buffer_write_io_vec(amqp_buffer_t *buffer, size_t n, int *iov_count)
{
    size_t block = buffer->limit.size >> AMQP_FRAGMENT_INDEX_BITS;
    size_t offset = buffer->limit.size & AMQP_FRAGMENT_INDEX_MASK;

    assert(n <= (buffer->capacity - buffer->limit.size));

    *iov_count = amqp_buffer_fill_io_vec(buffer, n, block, offset);

    return &buffer->io_vectors[0];
}

struct iovec *amqp_buffer_read_io_vec(amqp_buffer_t *buffer, int *iov_count)
{
    size_t block = buffer->limit.index >> AMQP_FRAGMENT_INDEX_BITS;
    size_t offset = buffer->limit.index & AMQP_FRAGMENT_INDEX_MASK;
    size_t n = buffer->limit.size - buffer->limit.index;

    assert(n <= (buffer->capacity - buffer->limit.index));

    *iov_count = amqp_buffer_fill_io_vec(buffer, n, block, offset);

    return &buffer->io_vectors[0];
}

void amqp_buffer_reference(amqp_buffer_t *buffer)
{
    buffer->reference_count++;
}
size_t amqp_buffer_reference_count(amqp_buffer_t *buffer)
{
    return buffer->reference_count;
}
size_t amqp_buffer_release(amqp_buffer_t *buffer)
{
    return  --buffer->reference_count;
}
