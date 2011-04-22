#include <stdio.h>

#include "Memory/Memory.h"
#include "Context/Context.h"
#include "Buffer/Buffer.h"

int amqp_context_default_debug_putc(int c)
{
    return fputc(c, stdout);
}

amqp_context_t *
amqp_new_amqp_context_t()
{
    amqp_context_t *result = AMQP_MALLOC(amqp_context_t);
    result->config.putc = amqp_context_default_debug_putc;

    // TODO - should be stderr
    result->debug.stream = stdout;
    result->debug.level = 10;

    result->decode.buffer = AMQP_ALLOC(amqp_buffer_t, result);
    result->convert_buffer = result->decode.buffer;
    result->encode.buffer = AMQP_ALLOC(amqp_buffer_t, result);
    return result;
}

void amqp_delete_amqp_context_t(amqp_context_t *context)
{
    if (context)
    {
        AMQP_DEALLOC(amqp_buffer_t, context, context->encode.buffer);
        AMQP_DEALLOC(amqp_buffer_t, context, context->decode.buffer);
        AMQP_FREE(context);
    }
}

amqp_debug_print_c_t *
amqp_context_define_putc_function(amqp_context_t *context, amqp_debug_print_c_t *putc)
{
    amqp_debug_print_c_t *old_value = context->config.putc;
    context->config.putc = putc;
    return old_value;
}

int amqp_context_putc(amqp_context_t *context, int c)
{
    return (*context->config.putc)(c);
}
