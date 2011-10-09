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

#ifndef LIBAMQP_MEMORY_MEMORY_H
#define LIBAMQP_MEMORY_MEMORY_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
    typedef struct amqp_context_t amqp_context_t;
#endif

#ifdef AMQP_MALLOC
#error libamqp redefines AMQP_MALLOC
#endif
#ifdef AMQP_FREE
#error libamqp redefines AMQP_FREE
#endif
#ifdef AMQP_MALLOC_ARRAY
#error libamqp redefines AMQP_MALLOC_ARRAY
#endif

#define AMQP_MALLOC(c, type)        ((type *) amqp_malloc((c), sizeof(type)))
#define AMQP_REALLOC(c, p, type)        ((type *) amqp_realloc((c), p, sizeof(type)))
#define AMQP_FREE(c, p)             (amqp_free((c), (p)), (p) = 0)
#define AMQP_MALLOC_ARRAY(c, t, n)  ((t *) amqp_malloc_array((c), sizeof(t), (n)))

    typedef struct amqp_allocation_stats_t
    {
        unsigned long outstanding_allocations;
        unsigned long total_allocation_calls;
    } amqp_allocation_stats_t;

    extern void *amqp_malloc(amqp_context_t *c, size_t n);
    extern void *amqp_malloc_array(amqp_context_t *c, size_t n, int count);
    extern void *amqp_realloc(amqp_context_t *c, const void *p, size_t n);
    extern void amqp_free(amqp_context_t *c, const void *p);

    extern void amqp_reset_malloc_allocation_stats();

#ifdef __cplusplus
}
#endif
#endif
