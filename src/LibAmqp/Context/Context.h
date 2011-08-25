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

#ifndef LIBAMQP_CONTEXT_CONTEXT_H
#define LIBAMQP_CONTEXT_CONTEXT_H

#include <stdio.h>

#include "libamqp.h"
#include "Memory/Pool.h"
#include "Buffer/Buffer.h"
#include "Codec/Type/Type.h"
#include "Context/ErrorHandling.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIBAMQP_AMQP_TYPE_T
#define LIBAMQP_AMQP_TYPE_T
typedef struct amqp_type_t amqp_type_t;
#endif

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_event_loop_t amqp_event_loop_t;
#endif

#ifndef LIBAMQP_AMQP_EVENT_LOOP_TYPE_T
#define LIBAMQP_AMQP_EVENT_LOOP_TYPE_T
typedef struct ev_loop amqp_event_loop_t;
#endif

typedef int amqp_debug_print_c_t(int c);

struct amqp_context_t
{
    int error_code;

    struct
    {
        amqp_debug_print_c_t *putc;
        int formatted_print;
        int use_name_for_composite_type;
        int continue_on_usage_error;
        int max_listen_queue_length;
    } config;

    struct
    {
        FILE *stream;
        int level;
        int indent;
        int last_char;
    } debug;

    struct
    {
        amqp_allocation_stats_t allocation_stats;
        amqp_memory_pool_t amqp_buffer_t_pool;
        amqp_memory_pool_t amqp_type_t_pool;
//        amqp_memory_pool_t amqp_frame_t_pool;
    } memory;

    struct {
        amqp_buffer_t *buffer;
    } decode;

    struct
    {
        amqp_buffer_t *buffer;
        amqp_type_t *container;
    } encode;

    amqp_event_loop_t *loop;
};

extern amqp_context_t *amqp_create_context();
extern amqp_context_t *amqp_context_clone(amqp_context_t *context);
extern int amqp_context_destroy(amqp_context_t *context);

extern amqp_type_t *amqp_allocate_type(amqp_context_t *context);
extern void amqp_deallocate_type(amqp_context_t *context, amqp_type_t *type);

extern amqp_buffer_t *amqp_allocate_buffer(amqp_context_t *context);
extern void amqp_deallocate_buffer(amqp_context_t *context, amqp_buffer_t *type);

extern amqp_debug_print_c_t *amqp_context_define_putc_function(amqp_context_t *context, amqp_debug_print_c_t *putc);
extern int amqp_context_printf(amqp_context_t *context, const char *format, ...);
extern int amqp_context_putc(amqp_context_t *context, int c);
extern int amqp_context_increase_print_indent(amqp_context_t *context, int delta);
extern int amqp_context_set_print_indent(amqp_context_t *context, int indent);

#ifdef __cplusplus
}
#endif
#endif
