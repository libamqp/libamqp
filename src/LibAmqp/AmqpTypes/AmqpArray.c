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
#include "AmqpTypes/AmqpArray.h"
#include "debug_helper.h"

static void release_memory(amqp_context_t *context, void **elements, amqp_array_t *array)
{
    if (elements)
    {
        AMQP_FREE(context, elements);
    }
    if (array)
    {
        AMQP_FREE(context, array);
    }
}

static void initialize(amqp_array_t *array, amqp_fn_table_t *table, amqp_type_t *type, size_t size, void **elements)
{
    array->leader.fn_table = table;
    array->type = type;
    array->capacity = array->size = size;
    array->elements = elements;
}

static void initialize_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_array_t *array = (amqp_array_t *) type;
    release_memory(context, array->elements, 0);
}
void amqp_array_initialize(amqp_context_t *context, amqp_array_t *array, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };
    initialize(array, &table, 0, size, AMQP_MALLOC_ARRAY(context, void *, size));
}

void amqp_array_initialize_from_type(amqp_context_t *context, amqp_array_t *array, amqp_type_t *type)
{
    assert(amqp_type_is_array(type));
    initialize(array, 0, type, type->value.array.count, (void **) type->value.array.elements);
}

static void create_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_array_t *array = (amqp_array_t *) type;
    release_memory(context, array->elements, array);
}
amqp_array_t *amqp_array_create(amqp_context_t *context, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };
    amqp_array_t *result = AMQP_MALLOC(context, amqp_array_t);
    initialize(result, &table, 0, size, AMQP_MALLOC_ARRAY(context, void *, size));
    return result;
}

static void create_with_type_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_array_t *array = (amqp_array_t *) type;
    release_memory(context, 0, array);
}
amqp_array_t *amqp_array_create_from_type(amqp_context_t *context, amqp_array_t *array, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = create_with_type_dtor
    };
    assert(amqp_type_is_array(type));

    amqp_array_t *result = AMQP_MALLOC(context, amqp_array_t);
    initialize(array, &table, type, type->value.array.count, (void **) type->value.array.elements);
    return result;
}
