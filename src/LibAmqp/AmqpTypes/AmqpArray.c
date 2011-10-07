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

static void array_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
not_implemented(todo);
}

void amqp_array_initialize(amqp_context_t *context, amqp_array_t *array, size_t size)
{
    static amqp_fn_table_t table = {
        .dtor = array_dtor
    };

    array->leader.fn_table = &table;

    array->type = 0;
    array->capacity = array->size = size;
    array->elements = AMQP_MALLOC_ARRAY(context, void *, size);
}
void amqp_array_initialize_from_type(amqp_context_t *context, amqp_array_t *array, amqp_type_t *type)
{
not_implemented(todo);
}
