#include <stdio.h>
#include <string.h>

#include "Buffer/Buffer.h"
#include "Type/Type.h"
#include "Codec/Decode.h"

char *amqp_convert_bytes_to_cstr(amqp_type_t *type)
{
    char *result = amqp_malloc(type->position.size + 1 TRACE_ARGS);
    strncpy(result, (const char *) &type->context->convert_buffer->bytes[type->position.index], type->position.size);
    return result;
}

