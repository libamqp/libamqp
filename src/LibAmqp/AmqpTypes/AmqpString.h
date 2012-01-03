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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_STRING_H
#define LIBAMQP_AMQP_TYPES_AMQP_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AmqpTypes/AmqpLeader.h"
#include "Codec/Type/TypeExtension.h"

#include "AmqpTypes/AmqpVariable.h"


struct amqp_string_t
{
    amqp_leader_t leader;
    amqp_variable_t v;
};

extern void amqp_string_initialize_as_null(amqp_context_t *context, amqp_string_t *string);
static inline
int amqp_string_is_null(amqp_string_t *string)
{
    return string->v.type == 0 && string->v.data == 0;
}

extern void amqp_string_initialize_from_type(amqp_context_t *context, amqp_string_t *string, amqp_type_t *type);
extern amqp_string_t *amqp_string_create(amqp_context_t *context, const char *data, size_t size);
extern void amqp_string_initialize(amqp_context_t *context, amqp_string_t *string, const char *data, size_t size);
static inline
amqp_string_t *amqp_string_create_from_cstr(amqp_context_t *context, const char *s)
{
    return amqp_string_create(context, s, strlen(s));
}
extern amqp_string_t *amqp_string_clone(amqp_context_t *context, amqp_string_t *source_string);

extern amqp_string_t *amqp_string_create_from_type(amqp_context_t *context,  amqp_type_t *type);

static inline
void amqp_string_cleanup(amqp_context_t *context, amqp_string_t *string)
{
    amqp_type_cleanup(context, (amqp_amqp_type_t *) string);
}

static inline
size_t amqp_string_size(amqp_string_t *string)
{
    return string->v.size;
}

extern int amqp_string_to_bytes(amqp_string_t *string, uint8_t *buffer, size_t buffer_size);

extern int amqp_string_compare(amqp_string_t *lhs, amqp_string_t *rhs);
extern int amqp_string_compare_with_cstr(amqp_string_t *lhs, const char *rhs);
extern int amqp_string_compare_with_bytes(amqp_string_t *lhs, const uint8_t *rhs, size_t size);

extern uint32_t amqp_string_hash(amqp_string_t *string);

extern int amqp_string_print(amqp_context_t *context, amqp_string_t *string);
extern const char *amqp_string_to_cstr(amqp_context_t *context, amqp_string_t *string);

#ifdef __cplusplus
}
#endif
#endif
