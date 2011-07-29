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

#include "platform_limits.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "libamqp_common.h"

#include "Context/ErrorHandling.h"
#include "Memory/Memory.h"
#include "Memory/Pool.h"
#include "Memory/PoolInternal.h"

#include "debug_helper.h"

#ifndef DISABLE_MEMORY_POOL
void amqp_pool_break()
{
    // nothing to do here
}

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
    size_t block_size = pool->allocation_size_in_bytes * pool->allocations_per_block + pool->offset_to_first_allocation;
    return amqp_malloc(block_size TRACE_ARGS);
}

static
amqp_memory_allocation_t *allocation_from_index(amqp_memory_pool_t *pool, amqp_memory_block_t *block, int index)
{
    return (amqp_memory_allocation_t *) &block->data.allocations[pool->allocation_size_in_bytes * index];
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
        allocation->header.leading_guard = (uint32_t) leading_mask;
        allocation->data.fragments[pool->object_size_in_fragments] = (size_t) trailing_mask;
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
    amqp_memory_allocation_t *free_allocation;
    amqp_memory_block_t *block_with_free_space = find_block_with_free_space((amqp_memory_block_t *) pool->block_list);
    if (block_with_free_space == 0)
    {
        block_with_free_space = allocate_new_memory_block(pool, &pool->block_list);
    }
    free_allocation = find_free_allocation_within_block(pool, block_with_free_space);

    if (free_allocation->header.block != block_with_free_space)
    {
	amqp_pool_break();
    	amqp_fatal_program_error("allocation from pool does not belong to correct block (set break on amqp_pool_break() to debug)");
    }

    return  &free_allocation->data;
}
#endif

void *amqp_allocate(amqp_memory_pool_t *pool)
{
    void *result;

    assert(pool != null);
    assert(pool->initialized);
    assert(pool->initializer_callback != null);

#ifdef DISABLE_MEMORY_POOL
    result = amqp_malloc(pool->object_size TRACE_ARGS);
#else
    assert(pool->object_size_in_fragments != 0);
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
    amqp_memory_allocation_t sample;
    size_t offset = (unsigned char *) &sample.data - (unsigned char *) &sample.header;
    return (amqp_memory_allocation_t *) ((unsigned char *) pooled_object - offset);
}

static
void delete_object(amqp_memory_pool_t *pool, void *pooled_object)
{
    amqp_memory_allocation_t *allocation = calculate_allocation_ptr_from_object_ptr(pool, pooled_object);
    amqp_memory_block_t *block;
    size_t index;
    int i, j;

    if (allocation->header.leading_guard != (uint32_t) leading_mask)
    {
	amqp_pool_break();
    	amqp_fatal_program_error("The leading guard on allocation has been corrupted. (set break on amqp_pool_break() to debug)");
    }
    if (allocation->data.fragments[pool->object_size_in_fragments] != (size_t) trailing_mask)
    {
	amqp_pool_break();
    	amqp_fatal_program_error("The trailing guard on allocation has been corrupted. (set break on amqp_pool_break() to debug)");
    }

    block = allocation->header.block;
    index = allocation->header.index;
    i = index / 8;
    j = index % 8;

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

static
void default_callback(amqp_memory_pool_t *pool, void *object)
{
    // nothing to see here, move along
}

#ifndef DISABLE_MEMORY_POOL

static
size_t calculate_block_offset_to_first_allocation()
{
    amqp_memory_block_t sample_block;
    size_t result = (unsigned char *) &sample_block.data - (unsigned char *) &sample_block.header;
    return result;
}

static
size_t calculate_offset_to_allocation_data()
{
    amqp_memory_block_t sample_allocation;
    size_t result = (unsigned char *) &sample_allocation.data - (unsigned char *) &sample_allocation.header;
    return result;
}

static
size_t byte_count_rounded_to_size_t_array_size(size_t n)
{
    size_t result = (n - 1) / SIZE_T_BYTES + 1;
    result = result < MIN_FRAGMENTS ? MIN_FRAGMENTS : result;
    return result;
}

static
size_t calculate_allocation_t_size_in_bytes(amqp_memory_pool_t *pool)
{
    size_t count = pool->object_size_in_fragments + 1;       // allow space for the trailing guard
    size_t result =  pool->offset_to_allocation_data + count * SIZE_T_BYTES;
    return result;
}

static size_t preferred_block_size();

static
size_t adjust_block_size(size_t block_size)
{
    size_t size = preferred_block_size() * 2;
    while (true)
    {
        size_t next_size = size >> 1;
        if (next_size < block_size)
        {
            break;
        }
        size = next_size;
    }
    return size;
}
#endif

static
size_t preferred_block_size()
{
    return LONG_BIT > 32 ? 2048 : 1024;
}

void amqp_initialize_pool_suggesting_block_size(amqp_memory_pool_t *pool, size_t pooled_object_size, size_t suggested_block_size)
{
    memset(pool, '\0', sizeof(amqp_memory_pool_t));
#ifndef DISABLE_MEMORY_POOL
    pool->block_size = adjust_block_size(suggested_block_size);

    pool->offset_to_first_allocation = calculate_block_offset_to_first_allocation();
    pool->offset_to_allocation_data = calculate_offset_to_allocation_data();
    pool->object_size_in_fragments = byte_count_rounded_to_size_t_array_size(pooled_object_size);
    pool->allocation_size_in_bytes = calculate_allocation_t_size_in_bytes(pool);

    do {
        pool->allocations_per_block = (pool->block_size - pool->offset_to_first_allocation) / pool->allocation_size_in_bytes;
        if (pool->allocations_per_block > LONG_BIT)
        {
            pool->block_size /= 2;
        }
    }
    while (pool->allocations_per_block > LONG_BIT);
    pool->allocations_mask = ((1UL << pool->allocations_per_block) & ~1UL) - 1;
#else
    pool->object_size = pooled_object_size;
#endif
    pool->initializer_callback = default_callback;
    pool->destroyer_callback = default_callback ;
    pool->initialized = true;
}

void amqp_initialize_pool(amqp_memory_pool_t *pool, size_t pooled_object_size)
{
    amqp_initialize_pool_suggesting_block_size(pool, pooled_object_size, preferred_block_size());
}

void amqp_pool_specify_initialization_callbacks(amqp_memory_pool_t *pool, amqp_pool_callback_t initializer_callback, amqp_pool_callback_t destroyer_callback)
{
    assert(pool->initialized);
    assert(pool->stats.total_allocation_calls == 0);

    pool->initializer_callback = initializer_callback;
    pool->destroyer_callback = destroyer_callback;
}

