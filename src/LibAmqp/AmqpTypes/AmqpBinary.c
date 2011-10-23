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

#include "AmqpTypes/AmqpTypesInternal.h"
#include "AmqpTypes/AmqpBinary.h"
#include "Codec/Type/TypeExtension.h"

#include "debug_helper.h"


void amqp_binary_initialize_as_null(amqp_context_t *context, amqp_binary_t *binary)
{
    not_implemented(todo);
}

void amqp_binary_initialize_from_type(amqp_context_t *context, amqp_binary_t *binary, amqp_type_t *type)
{
    assert(amqp_type_is_binary(type));

    binary->leader.fn_table = 0;
    binary->type = type;
    binary->size = type->position.size;
}

static void create_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_binary_t *binary = (amqp_binary_t *) type;

    AMQP_FREE(context, binary);
}
amqp_binary_t *amqp_binary_create_from_type(amqp_context_t *context, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };
    amqp_binary_t *result = AMQP_MALLOC(context, amqp_binary_t);

    assert(amqp_type_is_binary(type));

    result->leader.fn_table = &table;
    result->type = type;
    result->size = type->position.size;

    return result;
}
