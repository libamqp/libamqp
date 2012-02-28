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

#ifndef LIBAMQP_AMQP_TYPES_DECODE_AMQP_DECODE_INTERNAL_H
#define LIBAMQP_AMQP_TYPES_DECODE_AMQP_DECODE_INTERNAL_H

#include <stdlib.h>
#include "Context/Context.h"
#include "AmqpTypes/AmqpTypes.h"

#define AMQP_NO_DEFAULT_VALUE    0

#define AMQP_FIELD_DECODE_FN(ft) \
int amqp_decode_type_ ## ft(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_ ## ft ## _t *ft) \
{ \
    if (amqp_type_is_null(field)) \
    { \
        amqp_ ## ft ## _initialize_as_null(context, ft); \
        return true; \
    } \
    if (!amqp_type_is_ ## ft(field)) \
    { \
        amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type " #ft "."); \
        return false;  \
    } \
    amqp_ ## ft ## _initialize_from_type(context, ft, field); \
    return true; \
} \
int amqp_decode_mandatory_type_ ## ft(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, amqp_ ## ft ## _t *ft) \
{ \
    int rc = amqp_decode_type_ ## ft(context, field, field_number, total_fields, ft); \
    if (rc) \
    { \
        if (amqp_ ## ft ## _is_null(ft)) \
        { \
            amqp_mandatory_field_missing_error(context, field_number, total_fields, #ft); \
            return false; \
        } \
    } \
    return rc; \
}

#define AMQP_PRIMITIVE_FIELD_DECODE_FN(ft, t) \
static int \
decode_primitive_ ## ft(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, t *result) \
{ \
    if (!amqp_type_is_ ## ft(field)) \
    { \
        amqp_decode_field_error(context, field_number, total_fields, "Expected a field of type " #ft "."); \
        return false;  \
    } \
    *result = amqp_type_to_ ## ft(field); \
    return true; \
} \
int amqp_decode_primitive_ ## ft(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, int *null_flag, t *result, t default_value) \
{ \
    if (amqp_type_is_null(field)) \
    { \
        *result = default_value; \
        if (null_flag) \
        { \
            *null_flag = true; \
        } \
        return true; \
    } \
    return decode_primitive_ ## ft(context, field, field_number, total_fields, result); \
} \
int amqp_decode_mandatory_primitive_ ## ft(amqp_context_t *context, amqp_type_t *field, int field_number, int total_fields, t *result) \
{ \
    if (amqp_type_is_null(field)) \
    { \
        amqp_mandatory_field_missing_error(context, field_number, total_fields, #ft); \
        return false; \
    } \
    return decode_primitive_ ## ft(context, field, field_number, total_fields, result); \
}

enum {
    optional = false,
    mandatory = true
};

#define amqp_field_error(context, code, field, total, ...) _amqp_decode_field_error(context, __FILE__, __LINE__, #code, code, field, total, "" __VA_ARGS__)
#define amqp_decode_field_error(context, field, total, ...) amqp_field_error(context, AMQP_ERROR_FRAME_FIELD_DECODE_FAILED, field, total, "" __VA_ARGS__)
#define amqp_mandatory_field_missing_error(context, field, total, s) amqp_field_error(context, AMQP_ERROR_MULTIPLE_DECODE_FAILED, field, total, "Mandatory \"%s\" is null or missing", s)
#define amqp_field_range_error(context, field, total, ...) amqp_field_error(context, AMQP_ERROR_FRAME_FIELD_RANGE_ERROR, field, total, "" __VA_ARGS__)

#define amqp_decode_frame_error(context, frame_type) amqp_error(context, AMQP_ERROR_FRAME_DECODE_FAILED, "Failed to decode a \"%s\" frame.", frame_type);

static inline
amqp_type_t *amqp_field_from_list(amqp_type_t *list, int field_number)
{
    assert(amqp_type_is_list(list));
    return list && (field_number < list->value.list.count) ? list->value.list.elements[field_number] : 0;
}

void _amqp_decode_field_error(amqp_context_t *context, const char *filename, int line_number, const char *error_mnemonic, int error_code, int field, int total, const char *format, ...);


#endif
