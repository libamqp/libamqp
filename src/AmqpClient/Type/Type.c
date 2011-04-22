#include <stdio.h>
#include <string.h>

#include "ErrorHandling/ErrorHandling.h"
#include "Type/Type.h"
#include "Type/TypePool.h"
#include "Context/Context.h"


void amqp_mark_type_invalid(amqp_type_t *type, int cause)
{
    assert(type != 0);
    type->flags.is_invalid = 1;
    type->invalid_cause = cause;
}

amqp_type_t *
amqp_alloc_amqp_type_t(amqp_context_t *context)
{
    assert(context != 0);

    amqp_type_t *result = amqp_allocate__amqp_type_t(&context->pools.amqp_type_t_pool);
    result->extension_type_code = -1;

    return result;
}

void
amqp_dealloc_amqp_type_t(amqp_context_t *context, amqp_type_t *type)
{
    assert(context != 0);

    if (type)
    {
        if (type->value.compound.elements)
        {
            amqp_deallocate_amqp_type_t_array(context, type->value.compound.elements, type->value.compound.count);
        }
        amqp_delete__amqp_type_t(&context->pools.amqp_type_t_pool, type);
    }
}

amqp_type_t ** amqp_allocate_amqp_type_t_array(size_t count)
{
    size_t n = (count / 16 + 1) * 16;
    return (amqp_type_t **) amqp_malloc(n * sizeof(amqp_type_t *) TRACE_ARGS);
}

amqp_type_t ** amqp_realloc_amqp_type_t_array(amqp_type_t **elements, size_t count)
{
    size_t old_n = ((count - 1) / 16 + 1) * 16;
    size_t new_n = (count / 16 + 1) * 16;

    if (new_n > old_n)
    {
        elements = amqp_realloc(elements, new_n * sizeof(amqp_type_t *));
    }
    return elements;
}

void amqp_deallocate_amqp_type_t_array(amqp_context_t *context, amqp_type_t **array, size_t count)
{
    size_t i;

    if (array)
    {
        for (i = 0; i < count; i++)
        {
            amqp_dealloc_amqp_type_t(context, array[i]);
        }
        AMQP_FREE(array);
    }
}

bool amqp_type_match(amqp_type_t *lhs, amqp_type_t *rhs)
{
    if (lhs->format_code != rhs->format_code)
    {
        return false;
    }
    return true;
}

void amqp_describe_type(char *buffer, size_t size, amqp_type_t *type)
{
    amqp_type_meta_data_t *meta_data = type->meta_data;
    if (meta_data)
    {
        snprintf(buffer, size, "name=\"%s\", encoding=\"%s\", category=\"%s\"", meta_data->name, meta_data->encoding_name, amqp_category_names[meta_data->category_name_index]);
    }
    else
    {
        snprintf(buffer, size, "name=\"unknown\"");
    }
}
