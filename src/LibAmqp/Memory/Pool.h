/*
   Copyright 2011-2012 StormMQ Limited

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

#ifndef LIBAMQP_MEMORY_POOL_H
#define LIBAMQP_MEMORY_POOL_H

#include "platform_limits.h"

#include <stdlib.h>
#include "Context/DebugParams.h"
#include "Memory/Memory.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct amqp_memory_pool_t amqp_memory_pool_t;
typedef void (*amqp_pool_callback_t)(amqp_context_t *c, amqp_memory_pool_t *pool, void *object);

typedef struct amqp_memory_block_t amqp_memory_block_t;

#ifdef LIBAMQP_DEBUG_ALLOCATIONS
#ifndef LIBAMQP_AMQP_MEMORY_ALLOCATION_TYPE_T
#define LIBAMQP_AMQP_MEMORY_ALLOCATION_TYPE_T
    typedef struct amqp_memory_allocation_t amqp_memory_allocation_t;
#endif
#endif

struct amqp_memory_pool_t
{
#ifndef LIBAMQP_DISABLE_MEMORY_POOL
    amqp_memory_block_t *block_list;
    int allocations_per_block;
    unsigned long allocations_mask;
    size_t block_size;
    size_t offset_to_first_allocation;
    size_t offset_to_allocation_data;
    size_t object_size_in_fragments;    /* the size of the object being allocated from the pool */
    size_t allocation_size_in_bytes;     /* size of an allocation block: header + padding + object + trailing guard*/
#ifdef LIBAMQP_DEBUG_ALLOCATIONS
    amqp_memory_allocation_t *allocation_chain;
#endif /* LIBAMQP_DEBUG_ALLOCATIONS */
#else /* LIBAMQP_DISABLE_MEMORY_POOL */
    size_t object_size;
#endif /* LIBAMQP_DISABLE_MEMORY_POOL */
    amqp_allocation_stats_t stats;
    int initialized;
    amqp_pool_callback_t initializer_callback;
    amqp_pool_callback_t destroyer_callback;
    const char *name;
};

extern void amqp_initialize_pool(amqp_memory_pool_t *pool, size_t object_size, const char *pool_name);
extern void amqp_initialize_pool_suggesting_block_size(amqp_memory_pool_t *pool, size_t object_size, size_t suggested_block_size, const char *pool_name);
extern void amqp_pool_specify_initialization_callbacks(amqp_memory_pool_t *pool, amqp_pool_callback_t initializer_callback, amqp_pool_callback_t destroyer_callback);

#define amqp_allocate(c, p)     amqp__allocate((c), (p) AMQP_DEBUG_PARAMS)
extern void *amqp__allocate(amqp_context_t *c, amqp_memory_pool_t *pool AMQP_DEBUG_PARAMS_DECL);
extern void amqp_deallocate(amqp_context_t *c, amqp_memory_pool_t *pool, void *);

extern int amqp_check_pool_is_empty(amqp_context_t *context, amqp_memory_pool_t *pool);

#ifdef __cplusplus
}
#endif
#endif
