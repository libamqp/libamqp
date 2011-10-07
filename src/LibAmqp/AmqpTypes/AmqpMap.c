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
#include <string.h>

#include "Misc/Bits.h"
#include "Context/Context.h"

#include "AmqpTypes/AmqpMap.h"
#include "debug_helper.h"

void amqp_map_initialize(amqp_context_t *context, amqp_map_t *map, int initial_capacity, amqp_hash_fn_t hash, amqp_compare_fn_t compare)
{
    map->capacity = amqp_next_power_two(initial_capacity);
    assert(map->capacity > 0);

    map->count = 0;
    map->buckets = AMQP_MALLOC_ARRAY(context, amqp_entry_t *, map->capacity);
    map->entry_list = 0;
    map->hash = hash;
    map->compare = compare;
    map->on_heap = 0;
}

amqp_map_t *amqp_map_create(amqp_context_t *context, int initial_capacity, amqp_hash_fn_t hash, amqp_compare_fn_t compare)
{
    amqp_map_t *result = AMQP_MALLOC(context, amqp_map_t);
    amqp_map_initialize(context, result, initial_capacity, hash, compare);
    result->on_heap = 1;
    return result;
}

void amqp_map_cleanup_with_callback(amqp_context_t *context, amqp_map_t *map, amqp_free_callback_t callback)
{
    if (map)
    {
        amqp_entry_t *list = map->entry_list;

        while (list)
        {
            amqp_entry_t *entry = list;
            list = list->entry_list.next;
            if (callback)
            {
                callback(context, entry->key, entry->data);
            }
            AMQP_FREE(context, entry);
        }

        AMQP_FREE(context, map->buckets);
        map->buckets = 0;
        map->entry_list = 0;
        if (map->on_heap)
        {
            AMQP_FREE(context, map);
        }
    }
}

void amqp_map_cleanup(amqp_context_t *context, amqp_map_t *map)
{
    amqp_map_cleanup_with_callback(context, map, 0);
}

static amqp_entry_t *add_entry(amqp_context_t *context, amqp_map_t *map, int index, const void *key, const void *data)
{
    amqp_entry_t *entry = AMQP_MALLOC(context, amqp_entry_t);
    entry->key = key;
    entry->data = data;

    entry->collision_list.next = map->buckets[index];
    entry->collision_list.prev = &map->buckets[index];
    map->buckets[index] = entry;

    entry->entry_list.next = map->entry_list;
    entry->entry_list.prev = &map->entry_list;
    map->entry_list = entry;

    map->count++;
    return entry;
}

static amqp_entry_t *search_chain(amqp_map_t *map, amqp_entry_t *chain, const void *key)
{
    while (chain && map->compare(chain->key, key) != 0)
    {
        chain = chain->collision_list.next;
    }
    return chain;
}

static int calculate_index(amqp_map_t *map, const void *key)
{
    uint32_t hash = map->hash(key);
    uint32_t mask = map->capacity - 1;

    return hash & mask;
}

int amqp_map_put(amqp_context_t *context, amqp_map_t *map, const void *key, const void *data)
{
    int index;

    assert(map && map->hash && map->compare);
    
    index = calculate_index(map, key);

    if (map->buckets[index] != 0 && search_chain(map, map->buckets[index], key))
    {
        return false;
    }

    add_entry(context, map, index, key, data);

    return true;
}

const void *amqp_map_get(amqp_map_t *map, const void *key)
{
    int index = calculate_index(map, key);
    amqp_entry_t *entry = search_chain(map, map->buckets[index], key);
    return entry ? entry->data : 0;
}
