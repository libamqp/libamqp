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
#include "AmqpTypes/AmqpTypesInternal.h"
#include "AmqpTypes/AmqpMultiple.h"
#include "Codec/Type/TypeExtension.h"
#include "debug_helper.h"


static void initialize_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_multiple_symbol_t *multiple = (amqp_multiple_symbol_t *) type;
    if (multiple->symbols)
    {
        AMQP_FREE(context, multiple->symbols);
    }
}

static void create_dtor(amqp_context_t *context, amqp_amqp_type_t *type)
{
    amqp_multiple_symbol_t *multiple = (amqp_multiple_symbol_t *) type;
    if (multiple->symbols)
    {
        AMQP_FREE(context, multiple->symbols);
    }
    AMQP_FREE(context, multiple);
}

static
int initialize_from_type(amqp_context_t *context, amqp_multiple_symbol_t *multiple, amqp_type_t *type)
{
    multiple->type = type;

    if (amqp_type_is_null(type))
    {
        multiple->size = 0;
        multiple->symbols = 0;
        return true;
    }
    else if (amqp_type_is_symbol(type))
    {
        multiple->size = 1;
        multiple->symbols = AMQP_MALLOC_ARRAY(context, amqp_symbol_t, 1);
        amqp_symbol_initialize_from_type(context, &multiple->symbols[0], type);
        return true;
    }
    else if (amqp_type_is_array(type))
    {
        int i;

        // TODO -  deal with an empty array
        if (type->value.array.count > 0)
        {
            multiple->size = type->value.array.count;
            multiple->symbols = AMQP_MALLOC_ARRAY(context, amqp_symbol_t, multiple->size);

            for (i = 0; i < multiple->size; i++)
            {
                amqp_type_t *element = type->value.array.elements[i];
                if (!amqp_type_is_symbol(element))
                {
                    // TODO - if the first element is a symbol they all should be
                    return false;
                }
                amqp_symbol_initialize_from_type(context, &multiple->symbols[i], element);
            }
            return true;
        }
    }

    amqp_type_convert_set_failed(type);
    return false;
}

int amqp_multiple_symbol_initialize(amqp_context_t *context, amqp_multiple_symbol_t *multiple, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };
    multiple->leader.fn_table = &table;
    return initialize_from_type(context, multiple, type);
}

void amqp_multiple_symbol_initialize_as_null(amqp_context_t *context, amqp_multiple_symbol_t *multiple)
{
    static amqp_fn_table_t table = {
        .dtor = initialize_dtor
    };

    multiple->leader.fn_table = &table;
    multiple->size = 0;
    multiple->symbols = 0;
}

amqp_multiple_symbol_t *amqp_multiple_symbol_create(amqp_context_t *context, amqp_type_t *type)
{
    static amqp_fn_table_t table = {
        .dtor = create_dtor
    };
    amqp_multiple_symbol_t *result = AMQP_MALLOC(context, amqp_multiple_symbol_t);
    result->leader.fn_table = &table;
    initialize_from_type(context, result, type);
    return result;
}

int amqp_multiple_symbol_total_length(amqp_multiple_symbol_t *multiple)
{
    int result = 0;
    int i;
    for (i = 0; i < multiple->size; i++)
    {
        amqp_symbol_t *symbol = amqp_multiple_symbol_get(multiple, i);
        result += symbol->v.size;
    }
    return result;
}

int amqp_multiple_symbol_to_char_bytes(amqp_multiple_symbol_t *multiple, const char *spacer, char *buffer, size_t buffer_size)
{
    int result = 0;
    int available = buffer_size - 1;
    int i, n;
    char *p = buffer;

    if (buffer == 0 || buffer_size == 0)
    {
        return 0;
    }

    for (i = 0; i < multiple->size; i++)
    {
        if (i > 0)
        {
            n = snprintf(p, available, "%s", spacer);
            result += n;
            p += n;
            available -= n;
        }
        n = amqp_symbol_to_bytes(amqp_multiple_symbol_get(multiple, i), p, available);
        result += n;
        p += n;
        available -= n;
    }
    buffer[result] = '\0';
    return result;
}
