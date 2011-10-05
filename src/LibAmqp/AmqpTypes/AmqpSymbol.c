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


void amqp_symbol_initialize_reference(amqp_symbol_t *symbol, amqp_buffer_t *buffer, const unsigned char *reference, size_t size)
{
    if (buffer)
    {
        amqp_buffer_reference(buffer);
        symbol->buffer = buffer;
    }
    symbol->reference = reference;
    symbol->size = size;
}

void amqp_symbol_cleanup(amqp_symbol_t *symbol)
{
    if (symbol->buffer)
    {
        amqp_buffer_release(symbol->buffer);
    }
    symbol->reference = 0;
    symbol->size = 0;
}

int amqp_symbol_compare(amqp_symbol_t *lhs, amqp_symbol_t *rhs)
{
    int n, rc;
    assert(lhs && rhs);

    n = lhs->size;
    if (rhs->size < n) n = rhs->size;

    rc = memcmp(lhs->reference, rhs->reference, n);
    return rc != 0 ? rc : lhs->size - rhs->size;
}

int amqp_symbol_hash(amqp_symbol_t *symbol)
{
    return amqp_hash((const char *) symbol->reference, symbol->size);
}

