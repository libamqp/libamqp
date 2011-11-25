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
#include <string.h>
#include <stdio.h>

#include <unistd.h>

#include "Context/Context.h"

#include "Memory/Memory.h"
#include "Memory/Pool.h"
#include "AmqpTypes/AmqpDescriptor.h"

#include "debug_helper.h"

static char *generate_container_id(amqp_context_t *context);

typedef struct amqp__context_with_guard_t
{
    amqp_context_t context;
    uint64_t multiple_delete_protection;
} amqp__context_with_guard_t;

static const uint64_t random_sequence = 0xff31620854f9b573ULL; // generated on random.org

extern void amqp_type_initialize_pool(amqp_memory_pool_t *pool);
extern void amqp_frame_initialize_pool(amqp_memory_pool_t *pool);

static int amqp_context_default_debug_putc(int c)
{
    return fputc(c, stdout);
}

void amqp_restore_outputter(amqp_context_t *context)
{
    // TODO - should be stderr
    context->debug.arg.stream = stdout;
    context->debug.outputter = amqp_stream_outputter;
}

static const char *default_identity_hook(amqp_context_t *context)
{
    return 0;
}

amqp_context_t *
amqp_create_context()
{

    amqp__context_with_guard_t *result = AMQP_MALLOC(0, amqp__context_with_guard_t);

    result->context.config.putc = amqp_context_default_debug_putc;
    result->context.config.max_listen_queue_length = 5;

    amqp_restore_outputter(&result->context);
    result->context.debug.level = 10;
    result->multiple_delete_protection = random_sequence;

    amqp_buffer_initialize_pool(&result->context.memory.amqp_buffer_t_pool);
    amqp_type_initialize_pool(&result->context.memory.amqp_type_t_pool);
    amqp_frame_initialize_pool(&result->context.memory.amqp_frame_t_pool);

    result->context.limits.max_frame_size = AMQP_DEFAULT_MAX_FRAME_SIZE;
    result->context.limits.channel_max = AMQP_DEFAULT_CHANNEL_MAX;
    result->context.limits.idle_time_out = AMQP_DEFAULT_IDLE_TIMEOUT;

    result->context.thread_event_loop = 0;

    result->context.reference.plugins.sasl_plugin_list = 0;

    // Danger Will Robinson - using context while initialzing it. So, do last.
    result->context.reference.amqp_descriptors = amqp_load_descriptors(&result->context);
    result->context.reference.container_id = generate_container_id(&result->context);

    amqp_initialize_default_messaging_methods(&result->context.reference.plugins.messaging);

    amqp_context_register_identity_hooks(&result->context, default_identity_hook, default_identity_hook, default_identity_hook);

    return (amqp_context_t *) result;
}

amqp_context_t *amqp_context_clone(amqp_context_t *context)
{
    assert(context != 0);

    amqp__context_with_guard_t *result = AMQP_MALLOC(0, amqp__context_with_guard_t);

    result->context.config.putc = context->config.putc;
    result->context.config.max_listen_queue_length = context->config.max_listen_queue_length;

    amqp_restore_outputter(&result->context);
    result->context.debug.level = context->debug.level;
    result->multiple_delete_protection = random_sequence;

    // TODO - consider letting threads share the same set of pools, especially if memory is tight
    amqp_buffer_initialize_pool(&result->context.memory.amqp_buffer_t_pool);
    amqp_type_initialize_pool(&result->context.memory.amqp_type_t_pool);
    amqp_frame_initialize_pool(&result->context.memory.amqp_frame_t_pool);

    result->context.limits = context->limits;

    result->context.reference = context->reference;
    result->context.reference.cloned = true;

    result->context.thread_event_loop = 0;   // Don't clone the event loop. Need one per thread.

    // Don't clone the identity hooks.
    amqp_context_register_identity_hooks(&result->context, default_identity_hook, default_identity_hook, default_identity_hook);

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

int amqp_context_destroy(amqp_context_t *context)
{
    int pools_ok = true;
    int allocations_ok = true;

    if (context != 0)
    {
        if (((amqp__context_with_guard_t *) context)->multiple_delete_protection != random_sequence)
        {
            amqp_fatal_program_error("Attempting to destroy a Context twice.");
        }
        ((amqp__context_with_guard_t *) context)->multiple_delete_protection = 0;


        if (!context->reference.cloned)
        {
            amqp_context_free_sasl_plugins(context);
            amqp_descriptors_cleanup(context, context->reference.amqp_descriptors);
            amqp_free(context, context->reference.container_id);
        }

        pools_ok = check_pool(context, &context->memory.amqp_buffer_t_pool) &&
                check_pool(context, &context->memory.amqp_type_t_pool) &&
                check_pool(context, &context->memory.amqp_frame_t_pool);

        if (!(allocations_ok = context->memory.allocation_stats.outstanding_allocations == 0))
        {
            amqp_error(context, AMQP_ERROR_MEMORY_ERROR, "Number of calls to malloc does not match numbers of calls to free - %d outstanding allocations.", context->memory.allocation_stats.outstanding_allocations);
        }

        AMQP_FREE(0, context);
    }
    return pools_ok && allocations_ok;
}

amqp_type_t *amqp_allocate_type(amqp_context_t *context)
{
    return (amqp_type_t *) amqp_allocate(context, &context->memory.amqp_type_t_pool);
}

void amqp_deallocate_type(amqp_context_t *context, amqp_type_t *type)
{
    if (type != 0)
    {
        amqp_deallocate(context, &context->memory.amqp_type_t_pool, type);
    }
}

amqp_buffer_t *amqp_allocate_buffer(amqp_context_t *context)
{
    return (amqp_buffer_t *) amqp_allocate(context, &context->memory.amqp_buffer_t_pool);
}

void amqp_deallocate_buffer(amqp_context_t *context, amqp_buffer_t *buffer)
{
    if (buffer != 0)
    {
        amqp_deallocate(context, &context->memory.amqp_buffer_t_pool, buffer);
    }
}

amqp_frame_t *amqp_allocate_frame(amqp_context_t *context)
{
    return (amqp_frame_t *) amqp_allocate(context, &context->memory.amqp_frame_t_pool);
}

void amqp_deallocate_frame(amqp_context_t *context, amqp_frame_t *frame)
{
    if (frame != 0)
    {
        amqp_deallocate(context, &context->memory.amqp_frame_t_pool, frame);
    }
}

// TODO - lock error output stream
void amqp_stream_outputter(amqp_outputter_arg_t *dest, const char *filename, int line_number, const char *context_name, const char *label, const char *source, const char *extra, const char *format,
        va_list args)
{
    FILE *stream = dest->stream;

    flockfile(stream);

    if (filename)
    {
        fprintf(stream, line_number > 0 ? "%s:%d: " : "%s: ", filename, line_number);
    }
    if (context_name[0])
    {
        fprintf(stream, "%s: ", context_name);
    }
    if (label)
    {
        fprintf(stream, "%s: ", label);
    }
    if (source)
    {
        fprintf(stream, "%s: ", source);
    }
    if (extra)
    {
        fprintf(stream, "%s: ", extra);
    }
    if (format && *format)
    {
        vfprintf(stream, format, args);
    }
    fputc('\n', stream);
    fflush(stream);
    funlockfile(stream);
}

// TODO - lock error output stream
void amqp_buffer_outputter(amqp_outputter_arg_t *dest, const char *filename, int line_number, const char *context_name, const char *label, const char *source, const char *extra, const char *format, va_list args)
{
    char *buffer = dest->buffered.buffer;
    size_t buffer_size = dest->buffered.buffer_size;
    int n;

    if (context_name[0])
    {
        n = snprintf(buffer, buffer_size, "%s: ", context_name);
        buffer += n;
        buffer_size -= n;
    }
    if (label)
    {
        n = snprintf(buffer, buffer_size, "%s: ", label);
        buffer += n;
        buffer_size -= n;
    }

    if (source)
    {
        n = snprintf(buffer, buffer_size, "%s: ", source);
        buffer += n;
        buffer_size -= n;
    }

    if (extra)
    {
        n = snprintf(buffer, buffer_size, "%s: ", extra);
        buffer += n;
        buffer_size -= n;
    }

    if (format && *format)
    {
        vsnprintf(buffer, buffer_size, format, args);
    }
}

void amqp_output_to_buffer(amqp_context_t *context, char *buffer, size_t buffer_size)
{
    context->debug.outputter = amqp_buffer_outputter;
    context->debug.arg.buffered.buffer = buffer;
    context->debug.arg.buffered.buffer_size = buffer_size;
}

void amqp_context_set_name(amqp_context_t *context, const char *name)
{
    int n = sizeof(context->debug.name);
    strncpy(context->debug.name, name, n - 1);
    context->debug.name[n  - 1] = '\0';
}

uint8_t *amqp_allocate_print_buffer(amqp_context_t *context, size_t n)
{
    if (n < 128) n = 128;
    return (uint8_t *) malloc(n);
}
void amqp_deallocate_print_buffer(amqp_context_t *context, uint8_t *buffer)
{
    free(buffer);
}

static char *generate_container_id(amqp_context_t *context)
{
    const size_t space = HOST_NAME_MAX + 64;
    char *hostname = (char *) amqp_malloc(context, HOST_NAME_MAX + 1);
    size_t hostname_length;
    if (gethostname(hostname, HOST_NAME_MAX) == -1)
    {
        strcpy(hostname, "UNKNOWN");
    }
    hostname[HOST_NAME_MAX] = '\0';
    hostname_length = strlen(hostname);
    snprintf(&hostname[hostname_length], space - hostname_length, "-something-else-needed");
    return hostname;
}
