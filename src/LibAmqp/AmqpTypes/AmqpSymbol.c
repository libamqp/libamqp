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

#include "Buffer/Buffer.h"
#include "Misc/Bits.h"
#include "AmqpTypes/AmqpSymbol.h"


void amqp_symbol_initialize_reference(amqp_symbol_t *symbol, amqp_buffer_t *buffer, const char *reference, size_t size)
{
    if (buffer)
    {
        amqp_buffer_reference(buffer);
        symbol->buffer = buffer;
    }
    symbol->reference = reference;
    symbol->size = size;
}

amqp_symbol_t *amqp_symbol_create(amqp_context_t *context, amqp_buffer_t *buffer, const char *reference, size_t size)
{
    amqp_symbol_t *result = AMQP_MALLOC(context, amqp_symbol_t);
    result->on_heap = true;
    amqp_symbol_initialize_reference(result, buffer, reference, size);
    return result;
}

void amqp_symbol_cleanup(amqp_context_t *context, amqp_symbol_t *symbol)
{
    if (symbol->buffer)
    {
        amqp_buffer_release(symbol->buffer);
    }
    symbol->reference = 0;
    symbol->size = 0;
    if (symbol->on_heap)
    {
        AMQP_FREE(context, symbol);
    }
}

int amqp_symbol_compare(amqp_symbol_t *lhs, amqp_symbol_t *rhs)
{
    int n, rc;
    assert(lhs != 0 && rhs != 0);

    n = lhs->size;
    if (rhs->size < n) n = rhs->size;

    rc = memcmp(lhs->reference, rhs->reference, n);
    return rc != 0 ? rc : lhs->size - rhs->size;
}

uint32_t amqp_symbol_hash(amqp_symbol_t *symbol)
{
    assert(symbol != 0);
    return amqp_hash((void *) symbol->reference, symbol->size);
}

void amqp_symbol_map_initialize(amqp_context_t *context, amqp_map_t *map, int initial_capacity)
{
    amqp_map_initialize(context, map, initial_capacity, (amqp_hash_fn_t) amqp_symbol_hash, (amqp_compare_fn_t) amqp_symbol_compare);
}

static void cleanup_callback(amqp_context_t *context, const void *key, const void *data)
{
    amqp_symbol_t *symbol = (amqp_symbol_t *) key;
    AMQP_FREE(context, symbol);
}

void amqp_symbol_map_cleanup(amqp_context_t *context, amqp_map_t *map)
{
    amqp_map_cleanup_with_callback(context, map, cleanup_callback);
}

