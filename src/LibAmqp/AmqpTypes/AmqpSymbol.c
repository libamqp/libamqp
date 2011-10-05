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


void amqp_symbol_initialize_reference(amqp_symbol_t *symbol, amqp_buffer_t *buffer, const unsigned char *reference, size_t n)
{
    if (buffer)
    {
        amqp_buffer_reference(buffer);
        symbol->buffer = buffer;
    }
//not_implemented(todo);
}

void amqp_symbol_cleanup(amqp_symbol_t *symbol)
{
    if (symbol->buffer)
    {
        amqp_buffer_release(symbol->buffer);
    }
//not_implemented(todo);
}

