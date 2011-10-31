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
#include "Codec/Type/TypeExtension.h"
#include "AmqpTypes/AmqpVariableInternal.h"

#include "AmqpTypes/AmqpString.h"

#include "debug_helper.h"


void amqp_string_initialize_as_null(amqp_context_t *context, amqp_string_t *string)
{
    string->leader.fn_table = 0;
    amqp_variable_initialize_as_null(&string->v);
}

static void create_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_string_t *string = (amqp_string_t *) type;
    if (string->v.data)
    {
        AMQP_FREE(context, string->v.data);
    }

    AMQP_FREE(context, string);
}
static void initialize_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_string_t *string = (amqp_string_t *) type;
    if (string->v.data)
    {
        AMQP_FREE(context, string->v.data);
    }
}

void amqp_string_initialize_from_type(amqp_context_t *context, amqp_string_t *string, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };
    string->leader.fn_table = &table;

    assert(amqp_type_is_string(type));
    amqp_variable_initialize_from_type(&string->v, type);
}

amqp_string_t *amqp_string_create_from_type(amqp_context_t *context, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };
    amqp_string_t *result = AMQP_MALLOC(context, amqp_string_t);
    assert(amqp_type_is_string(type));

    result->leader.fn_table = &table;
    amqp_variable_initialize_from_type(&result->v, type);
    return result;
}

int amqp_string_to_bytes(amqp_string_t *string, uint8_t *buffer, size_t buffer_size)
{
    return amqp_variable_to_bytes(&string->v, buffer, buffer_size);
}

int amqp_string_compare(amqp_string_t *lhs, amqp_string_t *rhs)
{
    return amqp_variable_compare(&lhs->v, &rhs->v);
}

int amqp_string_compare_with_cstr(amqp_string_t *lhs, const char *rhs)
{
    return amqp_variable_compare_with_cstr(&lhs->v, rhs);
}

int amqp_string_compare_with_bytes(amqp_string_t *lhs, const uint8_t *rhs, size_t size)
{
    return amqp_variable_compare_with_bytes(&lhs->v, rhs, size);
}

uint32_t amqp_string_hash(amqp_string_t *string)
{
    return amqp_variable_hash(&string->v);
}

int amqp_string_print(amqp_context_t *context, amqp_string_t *string)
{
    int i;
    for (i = 0; i < string->v.size; i++)
    {
        amqp_context_putc(context, amqp_type_get_byte_at(string->v.type, i));
    }
    return i;
}
