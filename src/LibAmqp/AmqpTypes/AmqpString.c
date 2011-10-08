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
#include "AmqpTypes/AmqpString.h"
#include "Codec/Type/TypeExtension.h"

#include "debug_helper.h"


void amqp_string_initialize_from_type(amqp_context_t *context, amqp_string_t *string, amqp_type_t *type)
{
    assert(amqp_type_is_string(type));

    string->leader.fn_table = 0;
    string->type = type;
    string->size = type->position.size;
}

static void create_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_string_t *string = (amqp_string_t *) type;

    AMQP_FREE(context, string);
}
amqp_string_t *amqp_string_create_from_type(amqp_context_t *context, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };
    amqp_string_t *result = AMQP_MALLOC(context, amqp_string_t);

    assert(amqp_type_is_string(type));

    result->leader.fn_table = &table;
    result->type = type;
    result->size = type->position.size;

    return result;
}
