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
#include "AmqpTypes/AmqpTypesInternal.h"
#include "AmqpTypes/AmqpSymbol.h"
#include "Codec/Type/TypeExtension.h"


uint8_t *duplicate(amqp_context_t *context, const char *data, size_t size)
{
    uint8_t *result = (uint8_t *) amqp_malloc(context, size);
    memcpy(result, data, size);
    return result;
}

static void create_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_symbol_t *symbol = (amqp_symbol_t *) type;
    if (symbol->data)
    {
        AMQP_FREE(context, symbol->data);
    }

    AMQP_FREE(context, symbol);
}

static void initialize_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_symbol_t *symbol = (amqp_symbol_t *) type;
    if (symbol->data)
    {
        AMQP_FREE(context, symbol->data);
    }
}

void amqp_symbol_initialize(amqp_context_t *context, amqp_symbol_t *symbol, const char *data, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };
    symbol->leader.fn_table = &table;
    symbol->size = size;
    symbol->type = 0;
    symbol->data = duplicate(context, data, size);
}

amqp_symbol_t *amqp_symbol_create(amqp_context_t *context, const char *data, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };

    amqp_symbol_t *result = AMQP_MALLOC(context, amqp_symbol_t);
    result->leader.fn_table = &table;
    result->size = size;
    result->type = 0;
    result->data = duplicate(context, data, size);
    return result;
}

void amqp_symbol_initialize_from_type(amqp_context_t *context, amqp_symbol_t *symbol, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };
    symbol->leader.fn_table = &table;
    symbol->size = type->position.size;
    symbol->type = type;
    symbol->data = 0;
}

amqp_symbol_t *amqp_symbol_create_from_type(amqp_context_t *context, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };
    amqp_symbol_t *result = AMQP_MALLOC(context, amqp_symbol_t);
    result->leader.fn_table = &table;
    result->size = type->position.size;
    result->type = type;
    result->data = 0;
    return result;
}

int amqp_symbol_compare(amqp_symbol_t *lhs, amqp_symbol_t *rhs)
{
    int n, rc;
    assert(lhs != 0 && rhs != 0);

    n = lhs->size;
    if (rhs->size < n) n = rhs->size;

// TODO - delegate to type
    rc = memcmp(lhs->data, rhs->data, n);
    return rc != 0 ? rc : lhs->size - rhs->size;
}

uint32_t amqp_symbol_hash(amqp_symbol_t *symbol)
{
    assert(symbol != 0);
    // TODO - intrpduce a hash_type
    return amqp_hash((void *) symbol->data, symbol->size);
}

void amqp_symbol_map_initialize(amqp_context_t *context, amqp_map_t *map, int initial_capacity)
{
    amqp_map_initialize(context, map, initial_capacity, (amqp_hash_fn_t) amqp_symbol_hash, (amqp_compare_fn_t) amqp_symbol_compare);
}

amqp_map_t *amqp_symbol_map_create(amqp_context_t *context, int initial_capacity)
{
    return amqp_map_create(context, initial_capacity, (amqp_hash_fn_t) amqp_symbol_hash, (amqp_compare_fn_t) amqp_symbol_compare);
}

static void cleanup_callback(amqp_context_t *context, const void *key, const void *data)
{
    amqp_symbol_t *symbol = (amqp_symbol_t *) key;
    amqp_symbol_cleanup(context, symbol);
}

void amqp_symbol_map_cleanup(amqp_context_t *context, amqp_map_t *map)
{
    amqp_map_cleanup_with_callback(context, map, cleanup_callback);
}

