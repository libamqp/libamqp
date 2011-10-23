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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_SYMBOL_H
#define LIBAMQP_AMQP_TYPES_AMQP_SYMBOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "AmqpTypes/AmqpLeader.h"
#include "AmqpTypes/AmqpMap.h"
#include "AmqpTypes/AmqpVariable.h"


struct amqp_symbol_t
{
    amqp_leader_t leader;
    amqp_variable_t v;
};

extern void amqp_symbol_initialize_as_null(amqp_context_t *context, amqp_symbol_t *symbol);
extern void amqp_symbol_initialize(amqp_context_t *context, amqp_symbol_t *symbol, const char *data, size_t size);
extern amqp_symbol_t *amqp_symbol_create(amqp_context_t *context, const char *data, size_t size);
static inline void amqp_symbol_initialize_from_cstr(amqp_context_t *context, amqp_symbol_t *symbol, const char *s)
{
    amqp_symbol_initialize(context, symbol, s, strlen(s));
}
static inline amqp_symbol_t *amqp_symbol_create_from_cstr(amqp_context_t *context, const char *s)
{
    return amqp_symbol_create(context, s, strlen(s));
}
extern void amqp_symbol_initialize_from_type(amqp_context_t *context, amqp_symbol_t *symbol, amqp_type_t *type);
extern amqp_symbol_t *amqp_symbol_create_from_type(amqp_context_t *context, amqp_type_t *type);

static inline
void amqp_symbol_cleanup(amqp_context_t *context, amqp_symbol_t *symbol)
{
    amqp_type_cleanup(context, (amqp_amqp_type_t *) symbol);
}

extern void amqp_symbol_map_initialize(amqp_context_t *context, amqp_map_t *map, int initial_capacity);
extern amqp_map_t *amqp_symbol_map_create(amqp_context_t *context, int initial_capacity);
extern void amqp_symbol_map_cleanup(amqp_context_t *context, amqp_map_t *map);

static inline
int amqp_symbol_map_put(amqp_context_t *context, amqp_map_t *map, const amqp_symbol_t *key, const void *data)
{
    return amqp_map_put(context, map, key, data);
}
static inline
const void *amqp_symbol_map_get(amqp_map_t *map, const amqp_symbol_t *key)
{
    return amqp_map_get(map, key);
}

extern int amqp_symbol_to_bytes(amqp_symbol_t *symbol, uint8_t *buffer, size_t buffer_size);
extern int amqp_symbol_compare(amqp_symbol_t *lhs, amqp_symbol_t *rhs);
extern int amqp_symbol_compare_with_cstr(amqp_symbol_t *lhs, const char *rhs);
extern int amqp_symbol_compare_with_bytes(amqp_symbol_t *lhs, const uint8_t *rhs, size_t size);
extern uint32_t amqp_symbol_hash(amqp_symbol_t *symbol);

#ifdef __cplusplus
}
#endif
#endif
