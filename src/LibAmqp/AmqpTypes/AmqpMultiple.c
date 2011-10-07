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

#include "Context/Context.h"

#include "AmqpTypes/AmqpMultiple.h"
#include "debug_helper.h"


static void multiple_symbol_initialize(amqp_context_t *context, amqp_multiple_symbol_t *multiple, int size)
{
    multiple->size = size;
    if (size > AMQP_MULTIPLE_DEFAULT_SIZE)
    {
        multiple->array.indirect = AMQP_MALLOC_ARRAY(context, amqp_symbol_t *, size);
    }
}

void amqp_multiple_symbol_initialize(amqp_context_t *context, amqp_multiple_symbol_t *multiple, int size)
{
    memset(multiple, '\0', sizeof(amqp_multiple_symbol_t));
    multiple_symbol_initialize(context, multiple, size);
}

void amqp_multiple_symbol_cleanup(amqp_context_t *context, amqp_multiple_symbol_t *multiple)
{
    if (multiple && multiple->size > AMQP_MULTIPLE_DEFAULT_SIZE)
    {
        AMQP_FREE(context, multiple->array.indirect);
    }
}

amqp_multiple_symbol_t *amqp_multiple_symbol_create(amqp_context_t *context, int size)
{
    amqp_multiple_symbol_t *result = AMQP_MALLOC(context, amqp_multiple_symbol_t);
    multiple_symbol_initialize(context, result, size);
    return result;
}

void amqp_multiple_symbol_free(amqp_context_t *context, amqp_multiple_symbol_t *multiple)
{
    amqp_multiple_symbol_cleanup(context, multiple);
    AMQP_FREE(context, multiple);
}

