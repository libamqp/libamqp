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
#include "AmqpTypes/AmqpVariableInternal.h"

#include "debug_helper.h"

static void create_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_symbol_t *symbol = (amqp_symbol_t *) type;
    if (symbol->v.data)
    {
        AMQP_FREE(context, symbol->v.data);
    }

    AMQP_FREE(context, symbol);
}

static void initialize_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_symbol_t *symbol = (amqp_symbol_t *) type;
    if (symbol->v.data)
    {
        AMQP_FREE(context, symbol->v.data);
    }
}

void amqp_symbol_initialize_as_null(amqp_context_t *context, amqp_symbol_t *symbol)
{
    symbol->leader.fn_table = 0;
    amqp_variable_initialize_as_null(&symbol->v);
}

void amqp_symbol_initialize(amqp_context_t *context, amqp_symbol_t *symbol, const char *data, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };
    symbol->leader.fn_table = &table;
    amqp_variable_initialize(&symbol->v, (uint8_t *) amqp_duplicate(context, data, size), size);
}

static amqp_symbol_t *symbol_create(amqp_context_t *context, const uint8_t *data, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };

    amqp_symbol_t *result = AMQP_MALLOC(context, amqp_symbol_t);
    result->leader.fn_table = &table;
    amqp_variable_initialize(&result->v, data, size);
    return result;
}

amqp_symbol_t *amqp_symbol_create(amqp_context_t *context, const char *data, size_t size)
{
    return symbol_create(context, (uint8_t *) amqp_duplicate(context, data, size), size);
}

amqp_symbol_t *amqp_symbol_clone(amqp_context_t *context, amqp_symbol_t *source)
{
    return symbol_create(context, amqp_variable_clone_data(context, &source->v), source->v.size);
}

void amqp_symbol_initialize_from_type(amqp_context_t *context, amqp_symbol_t *symbol, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };
    assert(amqp_type_is_symbol(type));
    symbol->leader.fn_table = &table;
    amqp_variable_initialize_from_type(&symbol->v, type);
}

amqp_symbol_t *amqp_symbol_create_from_type(amqp_context_t *context, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };
    amqp_symbol_t *result = AMQP_MALLOC(context, amqp_symbol_t);
    assert(amqp_type_is_symbol(type));
    result->leader.fn_table = &table;
    amqp_variable_initialize_from_type(&result->v, type);
    return result;
}

void amqp_symbol_hash_table_initialize(amqp_context_t *context, amqp_hash_table_t *map, int initial_capacity)
{
    amqp_hash_table_initialize(context, map, initial_capacity, (amqp_hash_fn_t) amqp_symbol_hash, (amqp_compare_fn_t) amqp_symbol_compare);
}

amqp_hash_table_t *amqp_symbol_hash_table_create(amqp_context_t *context, int initial_capacity)
{
    return amqp_hash_table_create(context, initial_capacity, (amqp_hash_fn_t) amqp_symbol_hash, (amqp_compare_fn_t) amqp_symbol_compare);
}

static void cleanup_callback(amqp_context_t *context, const void *key, const void *data)
{
    amqp_symbol_t *symbol = (amqp_symbol_t *) key;
    amqp_symbol_cleanup(context, symbol);
}

void amqp_symbol_hash_table_cleanup(amqp_context_t *context, amqp_hash_table_t *map)
{
    amqp_hash_table_cleanup_with_callback(context, map, cleanup_callback);
}

int amqp_symbol_to_bytes(amqp_symbol_t *symbol, uint8_t *buffer, size_t buffer_size)
{
    return amqp_variable_to_bytes(&symbol->v, buffer, buffer_size);
}

int amqp_symbol_compare(amqp_symbol_t *lhs, amqp_symbol_t *rhs)
{
    return amqp_variable_compare(&lhs->v, &rhs->v);
}

int amqp_symbol_compare_with_cstr(amqp_symbol_t *lhs, const char *rhs)
{
    return amqp_variable_compare_with_cstr(&lhs->v, rhs);
}

int amqp_symbol_compare_with_bytes(amqp_symbol_t *lhs, const uint8_t *rhs, size_t size)
{
    return amqp_variable_compare_with_bytes(&lhs->v, rhs, size);
}

uint32_t amqp_symbol_hash(amqp_symbol_t *symbol)
{
    return amqp_variable_hash(&symbol->v);
}

