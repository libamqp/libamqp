#include "amqp_misc.h"
#include "Memory/Memory.h"
#include "Buffer/Buffer.h"
#include "Context/Context.h"
#include "Stream/Stream.h"
#include "ErrorHandling/ErrorHandling.h"


#include <string.h>

amqp_buffer_t *amqp_alloc_amqp_buffer_t(amqp_context_t *context)
{

    int size = 1024 * 4;
    amqp_buffer_t *result = AMQP_MALLOC(amqp_buffer_t);
    result->bytes = amqp_malloc(size TRACE_ARGS);
    result->actual_size = size;
    result->limit.index = 0;
    result->limit.size = 0;

    context->pools.amqp_buffer_t_pool.stats.outstanding_allocations++;
    context->pools.amqp_buffer_t_pool.stats.total_allocation_calls++;

    return result;
}

void amqp_dealloc_amqp_buffer_t(amqp_context_t *context, amqp_buffer_t *buffer)
{
    if (buffer)
    {
        context->pools.amqp_buffer_t_pool.stats.outstanding_allocations--;

        AMQP_FREE(buffer->bytes);
        AMQP_FREE(buffer);
    }
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

int amqp_buffer_read_from_stream(amqp_buffer_t *buffer, amqp_stream_t *stream, int n)
{
    assert(buffer != 0);
    assert(stream != 0);

    n = amqp_stream_read(stream, buffer->bytes, buffer->actual_size, buffer->limit.size, n);
    if (n > 0)
    {
        buffer->limit.size += n;
    }
    return n;
}

int amqp_buffer_write_to_stream(amqp_buffer_t *buffer, amqp_stream_t *stream)
{
    assert(buffer != 0);
    assert(stream != 0);

    return amqp_stream_write(stream, buffer->bytes, buffer->limit.size);
}


/*
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
        fatal_program_error();
    }

    return result;
}