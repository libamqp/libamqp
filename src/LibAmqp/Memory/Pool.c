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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "libamqp_common.h"

#include "Memory/Memory.h"
#include "Memory/Pool.h"
#include "Memory/PoolInternal.h"

#include "debug_helper.h"

#ifndef DISABLE_MEMORY_POOL

static
amqp_memory_block_t *find_block_with_free_space(amqp_memory_block_t *block_list)
{
    amqp_memory_block_t *result = block_list;
    while (result && result->header.mask.bits == 0)
    {
        result = result->header.next;
    }
    return result;
}

static
amqp_memory_block_t *allocate_block(amqp_memory_pool_t *pool)
{
    size_t block_size = pool->allocation_size * pool->allocations_per_block + sizeof(amqp_memory_block_header_t) + pool->block_data_padding;
    return amqp_malloc(block_size TRACE_ARGS);
}

static
amqp_memory_allocation_t *allocation_from_index(amqp_memory_pool_t *pool, amqp_memory_block_t *block, int index)
{
    return (amqp_memory_allocation_t *) &block->allocations_data[pool->allocation_size * index + pool->block_data_padding];
}

static
amqp_memory_block_t *allocate_new_memory_block(amqp_memory_pool_t *pool, amqp_memory_block_t **head)
{
    amqp_memory_block_t *result = allocate_block(pool);

    int i;
    if ((result->header.next = *head) != 0)
    {
        result->header.next->header.previous = result;
    }
    result->header.previous = 0;
    *head = result;

    result->header.mask.bits = -1UL & pool->allocations_mask;

    for (i = 0; i < pool->allocations_per_block; i++)
    {
        amqp_memory_allocation_t *allocation = allocation_from_index(pool, result, i);
        allocation->header.block = result;
        allocation->header.index = i;
    }
    return result;
}

static
unsigned find_first_free_allocation(unsigned char byte)
{
    int i;
    unsigned char mask = 0x01;
    for (i = 0; i < 8; i++)
    {
        if ((byte & mask) != 0)
        {
            return i;
        }
        mask <<= 1;
    }
    abort();
}

static
amqp_memory_allocation_t *find_free_allocation_within_block(amqp_memory_pool_t *pool, amqp_memory_block_t *block_with_free_space)
{
    int i;
    assert(block_with_free_space->header.mask.bits != 0);
    for (i = 0; i < (pool->allocations_per_block / 8 + 1); i++)
    {
        if (block_with_free_space->header.mask.bytes[i] != 0)
        {
            int j = find_first_free_allocation(block_with_free_space->header.mask.bytes[i]);
            int index = i * 8 + j;
            amqp_memory_allocation_t *free_allocation = allocation_from_index(pool, block_with_free_space, index);
            block_with_free_space->header.mask.bytes[i] = block_with_free_space->header.mask.bytes[i] & ~(1U << j);
            return free_allocation;
        }
    }
    abort();
}

static
void *allocate_object(amqp_memory_pool_t *pool)
{
    amqp_memory_block_t *block_with_free_space = find_block_with_free_space((amqp_memory_block_t *) pool->block_list);
    if (block_with_free_space == 0)
    {
        block_with_free_space = allocate_new_memory_block(pool, &pool->block_list);
    }
    amqp_memory_allocation_t *free_allocation = find_free_allocation_within_block(pool, block_with_free_space);

    assert(free_allocation->header.block == block_with_free_space);

    return  &free_allocation->data[pool->allocation_data_padding];
}
#endif

void *amqp_allocate(amqp_memory_pool_t *pool)
{
    void *result;

    assert(pool != null);
    assert(pool->initialized);
    assert(pool->object_size != 0);
    assert(pool->initializer_callback != null);

 #ifdef DISABLE_MEMORY_POOL
    result = amqp_malloc(pool->object_size TRACE_ARGS);
 #else
    result = allocate_object(pool);
 #endif

    (*pool->initializer_callback)(pool, result);

     pool->stats.outstanding_allocations++;
     pool->stats.total_allocation_calls++;

    return result;
}

#ifndef DISABLE_MEMORY_POOL

static
amqp_memory_allocation_t *calculate_allocation_ptr_from_object_ptr(amqp_memory_pool_t *pool, void *pooled_object)
{
    return (amqp_memory_allocation_t *) ((unsigned char *) pooled_object - sizeof(amqp_memory_allocation_header_t) - pool->allocation_data_padding);
}

static
void delete_object(amqp_memory_pool_t *pool, void *pooled_object)
{
    amqp_memory_allocation_t *allocation = calculate_allocation_ptr_from_object_ptr(pool, pooled_object);
    amqp_memory_block_t *block = allocation->header.block;
    size_t index = allocation->header.index;
    int i = index / 8;
    int j = index % 8;

    block->header.mask.bytes[i] = block->header.mask.bytes[i] | (1U << j);

    if ((~block->header.mask.bits & pool->allocations_mask) == 0)
    {
        if (block->header.next)
        {
            block->header.next->header.previous = block->header.previous;
        }
        if (block->header.previous)
        {
            block->header.previous->header.next = block->header.next;
        }
        else
        {
            pool->block_list = block->header.next;
        }
        amqp_free(block);
    }
}
#endif

void amqp_deallocate(amqp_memory_pool_t *pool, void *pooled_object)
{
    assert(pool != null);
    assert(pool->destroyer_callback != null);

    if (pooled_object != 0)
    {
        (*pool->destroyer_callback)(pool, pooled_object);
 #ifdef DISABLE_MEMORY_POOL
        amqp_free(pooled_object TRACE_ARGS);
 #else
        delete_object(pool, pooled_object);
 #endif
        pool->stats.outstanding_allocations--;
    }
}

static size_t calculate_safe_alignment()
{
    struct {
    	void *p;
	long double d;
    } struct_with_long_double;

    size_t  long_double_alignment = (unsigned char *) &struct_with_long_double.d - (unsigned char *) &struct_with_long_double.p;

    struct {
    	short s;
	double d;
    } struct_with_double;

    size_t  double_alignment = (unsigned char *) &struct_with_double.d - (unsigned char *) &struct_with_double.s;

    return long_double_alignment >= double_alignment ? long_double_alignment : double_alignment;
}

static
size_t round_object_size_to_alignment_boundary(size_t alignment, size_t pooled_object_size)
{
    return ((pooled_object_size - 1) / alignment + 1) * alignment;
}

static
size_t calculate_padding(size_t safe_alignment, size_t header_size)
{
    return (safe_alignment - header_size % safe_alignment) % safe_alignment;
}

static
size_t calculate_memory_allocation_t_size(amqp_memory_pool_t *pool)
{
    return sizeof(amqp_memory_allocation_header_t) + pool->allocation_data_padding + pool->object_size;
}

static void default_callback(amqp_memory_pool_t *pool, void *object)
{
    // nothing to see here, move along
}

void amqp_initialize_pool_specifing_block_limits(amqp_memory_pool_t *pool, size_t pooled_object_size, int allocations_per_block)
{
    if (pooled_object_size < sizeof(long))
    {
        pooled_object_size = sizeof(long);
    }
    
    if (allocations_per_block > LONG_BIT)
    {
        allocations_per_block = LONG_BIT;
    }

    memset(pool, '\0', sizeof(amqp_memory_pool_t));

    pool->allocations_per_block = allocations_per_block > 1 ? allocations_per_block : 2;
    pool->allocations_mask = ((1UL << pool->allocations_per_block) & ~1UL) - 1;

    pool->safe_alignment = calculate_safe_alignment();
    pool->object_size = round_object_size_to_alignment_boundary(pool->safe_alignment, pooled_object_size);

    pool->allocation_data_padding = calculate_padding(pool->safe_alignment, sizeof(amqp_memory_allocation_header_t));
    pool->block_data_padding = calculate_padding(pool->safe_alignment, sizeof(amqp_memory_block_header_t));

    pool->allocation_size = calculate_memory_allocation_t_size(pool);

    pool->initializer_callback = default_callback;
    pool->destroyer_callback = default_callback ;

    pool->initialized = true;
}

void amqp_initialize_pool(amqp_memory_pool_t *pool, size_t pooled_object_size)
{
    amqp_initialize_pool_specifing_block_limits(pool, pooled_object_size, LONG_BIT);
}

void amqp_initialize_pool_suggesting_block_size(amqp_memory_pool_t *pool, size_t object_size, size_t suggested_block_size)
{
    int n = ((suggested_block_size  - 1) / 512 + 1) * 512 / object_size;
    amqp_initialize_pool_specifing_block_limits(pool, object_size, n);
}

void amqp_pool_specify_initialization_callbacks(amqp_memory_pool_t *pool, amqp_pool_callback_t initializer_callback, amqp_pool_callback_t destroyer_callback)
{
    assert(pool->initialized);
    assert(pool->stats.total_allocation_calls == 0);

    pool->initializer_callback = initializer_callback;
    pool->destroyer_callback = destroyer_callback;
}

