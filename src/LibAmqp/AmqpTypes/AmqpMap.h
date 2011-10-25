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

#include "Codec/Type/Type.h"
#include "AmqpTypes/AmqpLeader.h"

struct amqp_map_t
{
    amqp_leader_t leader;
    amqp_type_t *type;
    size_t size;
    void **entries;
};

extern void amqp_map_initialize_as_null(amqp_context_t *context, amqp_map_t *map);
static inline
int amqp_map_is_null(amqp_map_t *map)
{
    return map->type == 0 && map->entries == 0;
}

extern void amqp_map_initialize(amqp_context_t *context, amqp_map_t *map, size_t size);
extern void amqp_map_initialize_from_type(amqp_context_t *context, amqp_map_t *map, amqp_type_t *type);
extern amqp_map_t *amqp_map_create(amqp_context_t *context, size_t size);
extern amqp_map_t *amqp_map_create_from_type(amqp_context_t *context, amqp_type_t *type);

static inline
void amqp_map_cleanup(amqp_context_t *context, amqp_map_t *map)
{
    amqp_type_cleanup(context, (amqp_amqp_type_t *) map);
}

static inline
size_t amqp_map_count(amqp_map_t *map)
{
    return map->size >> 1;
}

static inline
void amqp_map_key_set(amqp_map_t *map, size_t index, const void *p)
{
    assert(map->size > index && map->type == 0);
    map->entries[(index << 1)] = (void *) p;
}

static inline
void amqp_map_value_set(amqp_map_t *map, size_t index, const void *p)
{
    assert(map->size > index && map->type == 0);
    map->entries[(index << 1) + 1] = (void *) p;
}

static inline
void *amqp_map_key_get(amqp_map_t *map, size_t index)
{
    assert(map->size > index);
    index <<= 1;
    return map->type ? (void *) map->type->value.map.entries[index] : map->entries[index];
}

static inline
void *amqp_map_value_get(amqp_map_t *map, size_t index)
{
    assert(map->size > index);
    index = (index << 1) + 1;
    return map->type ? map->type->value.map.entries[index] : map->entries[index];
}

static inline
amqp_type_t *amqp_map_key_get_as_type(amqp_map_t *map, size_t index)
{
    assert(map->type);
    return (amqp_type_t *) amqp_map_key_get(map, index);
}

static inline
amqp_type_t *amqp_map_value_get_as_type(amqp_map_t *map, size_t index)
{
    assert(map->type);
    return (amqp_type_t *) amqp_map_value_get(map, index);
}

#ifdef __cplusplus
}
#endif
#endif
