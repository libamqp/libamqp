/*
   Copyright 2011-2012 StormMQ Limited

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
#include "AmqpTypes/AmqpArray.h"
#include "Codec/Type/TypeExtension.h"

#include "debug_helper.h"

void amqp_array_initialize_as_null(amqp_context_t *context, amqp_array_t *array)
{
    array->leader.fn_table = 0;
    array->type = 0;
    array->size = 0;
    array->elements = 0;
}

static void create_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_array_t *array = (amqp_array_t *) type;
    if (array->elements)
    {
        AMQP_FREE(context, array->elements);
    }

    AMQP_FREE(context, array);
}

static void initialize_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_array_t *array = (amqp_array_t *) type;
    if (array->elements)
    {
        AMQP_FREE(context, array->elements);
    }
}

void amqp_array_initialize(amqp_context_t *context, amqp_array_t *array, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };

    array->leader.fn_table = &table;
    array->type = 0;
    array->size = size;
    array->elements = AMQP_MALLOC_ARRAY(context, void *, size);
}

void amqp_array_initialize_from_type(amqp_context_t *context, amqp_array_t *array, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };

    assert(amqp_type_is_array(type));

    array->leader.fn_table = &table;
    array->type = type;
    array->size = type->value.array.count;
    array->elements = 0;
}

amqp_array_t *amqp_array_create(amqp_context_t *context, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };

    amqp_array_t *result = AMQP_MALLOC(context, amqp_array_t);
    result->leader.fn_table = &table;
    result->type = 0;
    result->size = size;
    result->elements = AMQP_MALLOC_ARRAY(context, void *, size);
    return result;
}

amqp_array_t *amqp_array_create_from_type(amqp_context_t *context, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };

    assert(amqp_type_is_array(type));

    amqp_array_t *result = AMQP_MALLOC(context, amqp_array_t);
    result->leader.fn_table = &table;
    result->type = type;
    result->size = type->value.array.count;
    result->elements = 0;
    return result;
}
