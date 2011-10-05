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

#include "libamqp_common.h"
#include "Context/Context.h"

#ifndef LIBAMQP_AMQP_SYMBOL_T
#define LIBAMQP_AMQP_SYMBOL_T
    typedef struct amqp_symbol_t amqp_symbol_t;
#endif

#define AMQP_MULTIPLE_DEFAULT_SIZE  8

typedef struct amqp_map_t
{
    size_t capacity;
    size_t count;
    void *buckets;
} amqp_map_t;

extern void amqp_map_initialize(amqp_context_t *context, amqp_map_t *map, int initial_capacity);
extern void amqp_map_cleanup(amqp_context_t *context, amqp_map_t *map);
extern amqp_map_t *amqp_map_create(amqp_context_t *context, int initial_capacity);
extern void amqp_map_free(amqp_context_t *context, amqp_map_t *map);

static inline size_t amqp_map_capacity(amqp_map_t *map)
{
    return map->capacity;
}

static inline size_t amqp_map_count(amqp_map_t *map)
{
    return map->count;
}

static inline double amqp_map_factor(amqp_map_t *map)
{
    return map->capacity ? (double) map->count / (double) map->capacity : 0.0;
}
#ifdef __cplusplus
}
#endif
#endif
