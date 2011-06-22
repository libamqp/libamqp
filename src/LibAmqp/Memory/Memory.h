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
extern "C" {
#endif

#include <stdlib.h>

#ifdef TRACE_ARGS
    #error libamqp redefines TRACE_ARGS
#endif
#ifdef TRACE_PARAMS
    #error libamqp redefines TRACE_PARAMS
#endif
#ifdef AMQP_MALLOC
    #error libamqp redefines AMQP_MALLOC
#endif
#ifdef AMQP_FREE
    #error libamqp redefines AMQP_FREE
#endif

#ifdef TRACE_ALLOCATIONS
#define TRACE_ARGS    , __FILE__, __LINE__
#define TRACE_PARAMS    , const char *fileName, int lineNumber
#else
#define TRACE_ARGS
#define TRACE_PARAMS
#endif

#define AMQP_MALLOC(type)     	    ((type *) amqp_malloc(sizeof(type) TRACE_ARGS))
#define AMQP_FREE(p)                (amqp_free(p TRACE_ARGS), p = 0)

typedef struct amqp_allocation_stats_t
{
    unsigned long outstanding_allocations;
    unsigned long total_allocation_calls;
} amqp_allocation_stats_t;

extern void *amqp_malloc(size_t n TRACE_PARAMS);
extern void *amqp_realloc(void *p, size_t n TRACE_PARAMS);
extern void amqp_free(void *p TRACE_PARAMS);

extern void amqp_reset_malloc_allocation_stats();
extern amqp_allocation_stats_t amqp_malloc_stats;
extern volatile int amqp_memory_trace_enabled;

extern int amqp_assert_that_calls_to_free_match_calls_to_malloc();
#ifdef __cplusplus
}
#endif
#endif
