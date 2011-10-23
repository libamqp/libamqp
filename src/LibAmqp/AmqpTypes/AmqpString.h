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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_STRING_H
#define LIBAMQP_AMQP_TYPES_AMQP_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AmqpTypes/AmqpLeader.h"
#include "Codec/Type/TypeExtension.h"

struct amqp_string_t
{
    amqp_leader_t leader;
    amqp_type_t *type;
    size_t size;
};

extern void amqp_string_initialize_as_null(amqp_context_t *context, amqp_string_t *string);
extern void amqp_string_initialize_from_type(amqp_context_t *context, amqp_string_t *string, amqp_type_t *type);
extern amqp_string_t *amqp_string_create_from_type(amqp_context_t *context,  amqp_type_t *type);

static inline
void amqp_string_cleanup(amqp_context_t *context, amqp_string_t *string)
{
    amqp_type_cleanup(context, (amqp_amqp_type_t *) string);
}

static inline
size_t amqp_string_size(amqp_string_t *string)
{
    return string->size;
}

static inline
size_t amqp_string_copy_to(amqp_string_t *string, uint8_t *destination, size_t destination_size)
{
    return amqp_type_copy_to(string->type, destination, destination_size);
}

#ifdef __cplusplus
}
#endif
#endif
