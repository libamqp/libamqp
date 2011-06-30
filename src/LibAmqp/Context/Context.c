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

#include <stdarg.h>
#include <stdio.h>
#include "Context/Context.h"

#include "Memory/Memory.h"
#include "Memory/Pool.h"

typedef struct amqp__context_with_guard_t
{
    amqp_context_t context;
    uint64_t multiple_delete_protection;
} amqp__context_with_guard_t;

static const uint64_t random_sequence = 0xff31620854f9b573;  // generated on random.org

/* declared in Codec/Type/Type.h */
extern void amqp_type_initialize_pool(amqp_memory_pool_t *pool);

int amqp_context_default_debug_putc(int c)
{
    return fputc(c, stdout);
}

amqp_context_t *
amqp_create_context()
{
    amqp__context_with_guard_t *result = AMQP_MALLOC(amqp__context_with_guard_t);

    result->context.config.putc = amqp_context_default_debug_putc;

    // TODO - should be stderr
    result->context.debug.stream = stdout;
    result->context.debug.level = 10;

    amqp_buffer_initialize_pool(&result->context.pools.amqp_buffer_t_pool);
    amqp_type_initialize_pool(&result->context.pools.amqp_type_t_pool);

    result->context.decode.buffer = amqp_allocate_buffer((amqp_context_t *) result);;
    result->context.encode.buffer = amqp_allocate_buffer((amqp_context_t *) result);;

    result->multiple_delete_protection = random_sequence;

    return (amqp_context_t *) result;
}

#define check_pool(context, pool) check_pool_allocations(context, pool, #pool)
static int check_pool_allocations(amqp_context_t *context, amqp_memory_pool_t *pool, const char *pool_name)
{
    if (pool->stats.outstanding_allocations != 0)
    {
        amqp_error(context, AMQP_ERROR_MEMORY_ERROR, "Pool  %s has %d outstanding allocations.", pool_name, pool->stats.outstanding_allocations);
        return false;
    }
    return true;
}

// amqp_destroy_context
int  amqp_destroy_context(amqp_context_t *context)
{
    int rc = true;
    if (context != 0)
    {
        if (((amqp__context_with_guard_t *) context)->multiple_delete_protection != random_sequence)
        {
            amqp_fatal_program_error("Attempting to destroy a Context twice.");
        }
        ((amqp__context_with_guard_t *) context)->multiple_delete_protection = 0;

        amqp_deallocate_buffer(context, context->encode.buffer);
        amqp_deallocate_buffer(context, context->decode.buffer);

        rc = check_pool(context, &context->pools.amqp_buffer_t_pool) &&
                check_pool(context, &context->pools.amqp_type_t_pool);

        AMQP_FREE(context);
    }
    return rc;
}

amqp_debug_print_c_t *
amqp_context_define_putc_function(amqp_context_t *context, amqp_debug_print_c_t *putc)
{
    amqp_debug_print_c_t *old_value = context->config.putc;
    context->config.putc = putc;
    return old_value;
}

int amqp_context_printf(amqp_context_t *context, const char *format, ...)
{
    char buffer[256];
    int i, n;

    va_list ap;
    va_start(ap, format);
    n = vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    for (i = 0; i < n; i++)
    {
        amqp_context_putc(context, buffer[i]);
    }

    return n;
}

static void putc_repeat(amqp_context_t *context, int c, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        (*context->config.putc)(c);
    }
}
int amqp_context_putc(amqp_context_t *context, int c)
{
    if (context->debug.last_char == '\n' && context->debug.indent > 0)
    {
        putc_repeat(context, '\t', context->debug.indent / 8);
        putc_repeat(context, ' ', context->debug.indent % 8);
    }
    context->debug.last_char = c;
    return (*context->config.putc)(c);
}

int amqp_context_increase_print_indent(amqp_context_t *context, int delta)
{
    int old_indent = context->debug.indent;
    context->debug.indent += delta;
    return old_indent;
}

int amqp_context_set_print_indent(amqp_context_t *context, int indent)
{
    int old_indent = context->debug.indent;
    context->debug.indent = indent;
    return old_indent;
}

amqp_type_t *amqp_allocate_type(amqp_context_t *context)
{
    return (amqp_type_t *) amqp_allocate(&context->pools.amqp_type_t_pool);
}

void amqp_deallocate_type(amqp_context_t *context, amqp_type_t *type)
{
    amqp_deallocate(&context->pools.amqp_type_t_pool, type);
}

amqp_buffer_t *amqp_allocate_buffer(amqp_context_t *context)
{
    return (amqp_buffer_t *) amqp_allocate(&context->pools.amqp_buffer_t_pool);
}

void amqp_deallocate_buffer(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_deallocate(&context->pools.amqp_buffer_t_pool, buffer);
}
