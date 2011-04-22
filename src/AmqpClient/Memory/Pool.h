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

#ifndef STORMMQ_AMQP_MEMORY_POOL_H
#define STORMMQ_AMQP_MEMORY_POOL_H
#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>

typedef struct amqp_allocation_stats_t 
{
    unsigned long outstanding_allocations;
    unsigned long total_allocation_calls;
} amqp_allocation_stats_t;
    
typedef struct amqp_memory_pool_t
{
    void *block_list;
    amqp_allocation_stats_t stats;
} amqp_memory_pool_t;


#define POOL_DECL(type) \
typedef struct amqp_memory_allocation_t__ ## type amqp_memory_allocation_t__ ## type; \
typedef struct amqp_memory_block_t__ ## type amqp_memory_block_t__ ## type; \
 \
struct amqp_memory_allocation_t__ ## type \
{ \
    amqp_memory_block_t__ ## type *block; \
    type data; \
}; \
struct amqp_memory_block_t__ ## type \
{ \
    amqp_memory_block_t__ ## type *next; \
    amqp_memory_block_t__ ## type *previous; \
    union \
    { \
        unsigned long bits; \
        unsigned char bytes[LONG_BIT/8]; \
    } l; \
    amqp_memory_allocation_t__ ## type allocations[LONG_BIT]; \
}; \
 \
extern type *amqp_allocate__ ## type(amqp_memory_pool_t *pool); \
extern void amqp_delete__ ## type(amqp_memory_pool_t *pool, type *); \
extern amqp_allocation_stats_t *amqp_memory_allocation__ ## type ## __hook;


#define POOL_DEFN(type) \
amqp_allocation_stats_t *amqp_memory_allocation__ ## type  ## __hook = 0; \
static amqp_memory_block_t__ ## type *allocate_new_memory_block(amqp_memory_block_t__ ## type **head) \
{ \
    amqp_memory_block_t__ ## type *amqp_memory_block = AMQP_MALLOC(amqp_memory_block_t__ ## type); \
    int i; \
    if ((amqp_memory_block->next = *head) != 0) \
    { \
        amqp_memory_block->next->previous = amqp_memory_block; \
    } \
    amqp_memory_block->previous = 0; \
    *head = amqp_memory_block; \
    amqp_memory_block->l.bits = -1L; \
    for (i = 0; i < LONG_BIT; i++) \
    { \
        amqp_memory_block->allocations[i].block = amqp_memory_block; \
    } \
    return amqp_memory_block; \
} \
static unsigned find_first_free_allocation(unsigned char byte) \
{ \
    int i; \
    unsigned char mask = 0x01; \
    for (i = 0; i < 8; i++) \
    { \
        if ((byte & mask) != 0) \
        { \
            return i; \
        } \
        mask <<= 1; \
    } \
    abort(); \
} \
static amqp_memory_block_t__ ## type *find_block_with_free_space(amqp_memory_block_t__ ## type *block_list) \
{ \
    amqp_memory_block_t__ ## type *result = block_list; \
    while (result && result->l.bits == 0) \
    { \
        result = result->next; \
    } \
    return result; \
} \
static amqp_memory_allocation_t__ ## type *find_free_allocation_within_block(amqp_memory_block_t__ ## type *block_with_free_space) \
{ \
    int i; \
    assert(block_with_free_space->l.bits != 0); \
    for (i = 0; i < LONG_BIT/8; i++) \
    { \
        if (block_with_free_space->l.bytes[i] != 0) \
        { \
            amqp_memory_allocation_t__ ## type *free_allocation; \
            int j = find_first_free_allocation(block_with_free_space->l.bytes[i]); \
            int index = i * (LONG_BIT/8) + j; \
            block_with_free_space->l.bytes[i] = block_with_free_space->l.bytes[i] & ~(1U << j); \
            free_allocation = &block_with_free_space->allocations[index]; \
            return free_allocation; \
        } \
    } \
    abort(); \
} \
type *amqp_allocate__ ## type(amqp_memory_pool_t *pool) \
{ \
    amqp_memory_block_t__ ## type *block_with_free_space = find_block_with_free_space((amqp_memory_block_t__ ## type *) pool->block_list); \
    if (block_with_free_space == 0) \
    { \
        block_with_free_space = allocate_new_memory_block((amqp_memory_block_t__ ## type **) &pool->block_list); \
    } \
    amqp_memory_allocation_t__ ## type *free_allocation = find_free_allocation_within_block(block_with_free_space); \
    assert(free_allocation->block == block_with_free_space); \
    if (amqp_memory_allocation__ ## type ## __hook) \
    { \
        amqp_memory_allocation__ ## type ## __hook->outstanding_allocations++; \
        amqp_memory_allocation__ ## type ## __hook->total_allocation_calls++; \
    } \
    pool->stats.outstanding_allocations++; \
    pool->stats.total_allocation_calls++; \
    return &free_allocation->data; \
} \
static inline amqp_memory_allocation_t__ ## type *amqp_to_allocation_t_p(type *t) \
{ \
    amqp_memory_allocation_t__ ## type *ap = (amqp_memory_allocation_t__ ## type *) ((unsigned char *) t - sizeof (amqp_memory_block_t__ ## type *)); \
    return ap; \
} \
void amqp_delete__ ## type(amqp_memory_pool_t *pool, type *t) \
{ \
    amqp_memory_allocation_t__ ## type *allocation = amqp_to_allocation_t_p(t); \
    amqp_memory_block_t__ ## type *block = allocation->block; \
    size_t index = allocation - &block->allocations[0]; \
    unsigned long mask = ((unsigned long) 0x01) << index; \
    block->l.bits = block->l.bits | mask; \
    if (~block->l.bits == 0) \
    { \
        if (block->next) \
        { \
            block->next->previous = block->previous; \
        } \
        if (block->previous) \
        { \
            block->previous->next = block->next; \
        } \
        else \
        { \
            pool->block_list = block->next; \
        } \
        amqp_free(block); \
    } \
    if (amqp_memory_allocation__ ## type ## __hook) \
    { \
        amqp_memory_allocation__ ## type ## __hook->outstanding_allocations--; \
        amqp_memory_allocation__ ## type ## __hook->total_allocation_calls--; \
    } \
    pool->stats.outstanding_allocations--; \
    pool->stats.total_allocation_calls--; \
}

#define POOL_TESTS(module, type) \
namespace \
{ \
    amqp_allocation_stats_t stats; \
    static void block_count_reset() \
    { \
        stats.outstanding_allocations = 0; \
        stats.total_allocation_calls = 0; \
        amqp_memory_allocation__ ## type ## __hook = &stats; \
    } \
    static void block_count_check(Test& test, TestResult& result) \
    { \
        if (stats.outstanding_allocations > 0) \
        { \
            test.addFailureTo(result, "Calls to amqp_alloc_" #type "() and amqp_dealloc_" #type "() do not match"); \
        } \
    } \
    TestHook hook(block_count_reset, block_count_check); \
    t::Context *c; \
    void SetUp() \
    { \
        c = new t::Context(); \
    } \
    void TearDown() \
    { \
        delete c; \
    } \
    type *allocate() \
    { \
        return amqp_allocate__ ## type(&c->context()->pools.type ## _pool); \
    } \
    void deallocate(type *t) \
    { \
        amqp_delete__ ## type(&c->context()->pools.type ## _pool, t); \
    } \
 \
    amqp_memory_block_t__ ## type *list() \
    { \
        return (amqp_memory_block_t__ ## type *) c->context()->pools.type ## _pool.block_list; \
    } \
} \
TEST(module, allocation_count) \
{ \
    type *v = allocate(); \
    ASSERT_EQUAL(1LU, stats.outstanding_allocations); \
    ASSERT_EQUAL(1LU, c->context()->pools.type ## _pool.stats.outstanding_allocations); \
 \
    deallocate(v); \
    ASSERT_EQUAL(0LU, stats.outstanding_allocations); \
    ASSERT_EQUAL(0LU, c->context()->pools.type ## _pool.stats.outstanding_allocations); \
} \
TEST(module, allocate_lots) \
{ \
    type *types[130]; \
    int i; \
 \
    for (i = 0; i < (64 * 2) + 1; i++) \
    { \
        types[i] = allocate(); \
        ASSERT_NOT_NULL(types[i]); \
    } \
 \
    while (i > 0) \
    { \
        i--; \
        deallocate(types[i]); \
    } \
 \
    ASSERT_EQUAL(0, list()); \
} \
TEST(module, AllocateAndFree) \
{ \
    type *t, *t1; \
    t = allocate(); \
    ASSERT_NOT_NULL(t); \
    ASSERT_NOT_NULL(list()); \
    deallocate(t); \
    ASSERT_NULL(list()); \
 \
    t = allocate(); \
    ASSERT_NOT_NULL(t); \
    ASSERT_NOT_NULL(list()); \
    deallocate(t); \
    ASSERT_NULL(list()); \
 \
    t = allocate(); \
    ASSERT_NOT_NULL(t); \
    ASSERT_NOT_NULL(list()); \
    t1 = allocate(); \
    ASSERT_NOT_NULL(t1); \
    deallocate(t1); \
    deallocate(t); \
    ASSERT_NULL(list()); \
 \
    t = allocate(); \
    ASSERT_NOT_NULL(t); \
    ASSERT_NOT_NULL(list()); \
    t1 = allocate(); \
    ASSERT_NOT_NULL(t1); \
    deallocate(t); \
    deallocate(t1); \
    ASSERT_NULL(list()); \
}

#ifdef __cplusplus
}
#endif
#endif
