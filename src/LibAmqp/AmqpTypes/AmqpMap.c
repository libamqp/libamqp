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

#include "AmqpTypes/AmqpMap.h"
#include "debug_helper.h"

/*
static void map_initialize(amqp_context_t *context, amqp_map_t *map, int size)
{
    map->size = size;
    if (size > AMQP_MULTIPLE_DEFAULT_SIZE)
    {
        map->array.indirect = AMQP_MALLOC_ARRAY(context, amqp_symbol_t *, size);
    }
}

void amqp_map_initialize(amqp_context_t *context, amqp_map_t *map, int size)
{
    memset(map, '\0', sizeof(amqp_map_t));
    map_initialize(context, map, size);
}

void amqp_map_cleanup(amqp_context_t *context, amqp_map_t *map)
{
    if (map->size > AMQP_MULTIPLE_DEFAULT_SIZE)
    {
        AMQP_FREE(context, map->array.indirect);
    }
}

amqp_map_t *amqp_map_create(amqp_context_t *context, int size)
{
    amqp_map_t *result = AMQP_MALLOC(context, amqp_map_t);
    map_initialize(context, result, size);
    return result;
}

void amqp_map_free(amqp_context_t *context, amqp_map_t *map)
{
    amqp_map_cleanup(context, map);
    AMQP_FREE(context, map);
}

*/