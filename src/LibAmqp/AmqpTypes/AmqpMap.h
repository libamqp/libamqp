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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_MAP_H
#define LIBAMQP_AMQP_TYPES_AMQP_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "libamqp_common.h"
#include "AmqpTypes/AmqpHash.h"
#include "Context/Context.h"


#ifndef LIBAMQP_AMQP_MAP_TYPE_T
#define LIBAMQP_AMQP_MAP_TYPE_T
typedef struct amqp_map_t amqp_map_t;
#endif

typedef struct amqp_entry_t amqp_entry_t;
struct amqp_entry_t
{
    struct {
        amqp_entry_t *next;
        amqp_entry_t **prev;
    } collision_list;
    struct {
        amqp_entry_t *next;
        amqp_entry_t **prev;
    } entry_list;
    const void *key;
    const void *data;
};

typedef uint32_t (*amqp_hash_fn_t)(const void *key);
typedef int (*amqp_compare_fn_t)(const void *lhs, const void *rhs);
typedef void (*amqp_free_callback_t)(amqp_context_t *context, const void *key, const void *data);

struct amqp_map_t
{
    size_t capacity;
    size_t count;
    amqp_entry_t **buckets;
    amqp_entry_t *entry_list;
    amqp_hash_fn_t hash;
    amqp_compare_fn_t compare;
    int on_heap;
};

extern void amqp_map_initialize(amqp_context_t *context, amqp_map_t *map, int initial_capacity, amqp_hash_fn_t hash, amqp_compare_fn_t compare);
extern amqp_map_t *amqp_map_create(amqp_context_t *context, int initial_capacity, amqp_hash_fn_t hash, amqp_compare_fn_t compare);
extern void amqp_map_cleanup(amqp_context_t *context, amqp_map_t *map);
extern void amqp_map_cleanup_with_callback(amqp_context_t *context, amqp_map_t *map, amqp_free_callback_t callback);

extern int amqp_map_put(amqp_context_t *context, amqp_map_t *map, const void *key, const void *data);
extern const void *amqp_map_get(amqp_map_t *map, const void *key);

static inline
size_t amqp_map_capacity(amqp_map_t *map)
{
    return map->capacity;
}

static inline
size_t amqp_map_count(amqp_map_t *map)
{
    return map->count;
}

static inline
double amqp_map_factor(amqp_map_t *map)
{
    return map->capacity ? (double) map->count / (double) map->capacity : 0.0;
}

#ifdef __cplusplus
}
#endif
#endif
