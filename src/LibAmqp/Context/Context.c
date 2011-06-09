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

#include <stdio.h>

#include "Context/Context.h"

#include "Memory/Memory.h"
#include "Memory/Pool.h"

int amqp_context_default_debug_putc(int c)
{
    return fputc(c, stdout);
}

amqp_context_t *
amqp_create_context()
{
    amqp_context_t *result = AMQP_MALLOC(amqp_context_t);
    result->config.putc = amqp_context_default_debug_putc;

    // TODO - should be stderr
    result->debug.stream = stdout;
    result->debug.level = 10;

    amqp_buffer_initialize_pool(&result->pools.amqp_buffer_t_pool);
    amqp_buffer_initialize_pool(&result->pools.amqp_type_t_pool);

    result->decode.buffer = (amqp_buffer_t *) amqp_allocate(&result->pools.amqp_buffer_t_pool);
    result->encode.buffer = (amqp_buffer_t *) amqp_allocate(&result->pools.amqp_buffer_t_pool);

    result->convert_buffer = result->decode.buffer; // TODO - stop using an alias
    return result;
}

// amqp_destroy_context
void amqp_destroy_context(amqp_context_t *context)
{
    if (context)
    {
        amqp_deallocate(&context->pools.amqp_buffer_t_pool, context->encode.buffer);
        amqp_deallocate(&context->pools.amqp_buffer_t_pool, context->decode.buffer);
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
