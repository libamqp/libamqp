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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_HASH_TABLE_H
#define LIBAMQP_AMQP_TYPES_AMQP_HASH_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AmqpTypes/AmqpLeader.h"
#include "AmqpTypes/AmqpEntry.h"

struct amqp_hash_table_t
{
    amqp_leader_t leader;
    size_t capacity;
    size_t count;
    amqp_entry_t **buckets;
    amqp_entry_t *entry_list;
    amqp_hash_fn_t hash;
    amqp_compare_fn_t compare;
    int on_heap;
};

extern void amqp_hash_table_initialize(amqp_context_t *context, amqp_hash_table_t *map, int initial_capacity, amqp_hash_fn_t hash, amqp_compare_fn_t compare);
extern amqp_hash_table_t *amqp_hash_table_create(amqp_context_t *context, int initial_capacity, amqp_hash_fn_t hash, amqp_compare_fn_t compare);
extern void amqp_hash_table_cleanup(amqp_context_t *context, amqp_hash_table_t *map, amqp_free_callback_t callback);

extern int amqp_hash_table_put(amqp_context_t *context, amqp_hash_table_t *map, const void *key, const void *data);
extern const void *amqp_hash_table_get(amqp_hash_table_t *map, const void *key);

static inline
size_t amqp_hash_table_capacity(amqp_hash_table_t *map)
{
    return map->capacity;
}

static inline
size_t amqp_hash_table_count(amqp_hash_table_t *map)
{
    return map->count;
}

static inline
double amqp_hash_table_factor(amqp_hash_table_t *map)
{
    return map->capacity ? (double) map->count / (double) map->capacity : 0.0;
}

#ifdef __cplusplus
}
#endif
#endif
