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

#ifndef LIBAMQP_CONTEXT_H
#define LIBAMQP_CONTEXT_H

#include <stdio.h>

#include "libamqp_common.h"
#include "Memory/Pool.h"
#include "Buffer/Buffer.h"
#include "Context/ErrorHandling.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIBAMQP_AMQP_TYPE_T
#define LIBAMQP_AMQP_TYPE_T
typedef struct amqp_type_t amqp_type_t;
#endif

#ifndef LIBAMQP_CONTEXT_TYPE
#define LIBAMQP_CONTEXT_TYPE
typedef struct amqp_context_t amqp_context_t;
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
    } config;

    struct
    {
        FILE *stream;
        int level;
    } debug;

    struct
    {
        amqp_memory_pool_t amqp_buffer_t_pool;
        amqp_memory_pool_t amqp_type_t_pool;
//        amqp_memory_pool_t amqp_frame_t_pool;
    } pools;

    struct {
        amqp_buffer_t *buffer;
    } decode;

    struct
    {
        amqp_buffer_t *buffer;
        amqp_type_t *container;
    } encode;

    amqp_buffer_t *convert_buffer;
};

extern amqp_context_t *amqp_create_context();
extern void amqp_destroy_context(amqp_context_t *context);

extern int amqp_context_default_debug_putc(int c);
extern amqp_debug_print_c_t *amqp_context_define_putc_function(amqp_context_t *context, amqp_debug_print_c_t *putc);
extern int amqp_context_putc(amqp_context_t *context, int c);

#ifdef __cplusplus
}
#endif
#endif
