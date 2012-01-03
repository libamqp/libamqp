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

#include <assert.h>
#include <string.h>

#include "AmqpTypes/AmqpTypesInternal.h"
#include "AmqpTypes/AmqpMap.h"
#include "Codec/Type/TypeExtension.h"

#include "debug_helper.h"


void amqp_map_initialize_as_null(amqp_context_t *context, amqp_map_t *map)
{
    map->leader.fn_table = 0;
    map->type = 0;
    map->size = 0;
    map->entries = 0;
}

static void create_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_map_t *map = (amqp_map_t *) type;
    if (map->entries)
    {
        AMQP_FREE(context, map->entries);
    }

    AMQP_FREE(context, map);
}

static void initialize_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_map_t *map = (amqp_map_t *) type;
    if (map->entries)
    {
        AMQP_FREE(context, map->entries);
    }
}

void amqp_map_initialize(amqp_context_t *context, amqp_map_t *map, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };

    assert(size == (size & ~1));

    map->leader.fn_table = &table;
    map->type = 0;
    map->size = size;
    map->entries = AMQP_MALLOC_ARRAY(context, void *, size);
}

void amqp_map_initialize_from_type(amqp_context_t *context, amqp_map_t *map, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };

    assert(amqp_type_is_map(type));

    map->leader.fn_table = &table;
    map->type = type;
    map->size = type->value.map.count;
    map->entries = 0;
}

amqp_map_t *amqp_map_create(amqp_context_t *context, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };

    assert(size == (size & ~1));

    amqp_map_t *result = AMQP_MALLOC(context, amqp_map_t);
    result->leader.fn_table = &table;
    result->type = 0;
    result->size = size;
    result->entries = AMQP_MALLOC_ARRAY(context, void *, size);
    return result;
}

amqp_map_t *amqp_map_create_from_type(amqp_context_t *context, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };

    assert(amqp_type_is_map(type));

    amqp_map_t *result = AMQP_MALLOC(context, amqp_map_t);
    result->leader.fn_table = &table;
    result->type = type;
    result->size = type->value.map.count;
    result->entries = 0;
    return result;
}
