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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_VARIABLE_INTERNAL_H
#define LIBAMQP_AMQP_TYPES_AMQP_VARIABLE_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "libamqp_common.h"
#include "AmqpTypes/AmqpLeader.h"
#include "Codec/Type/Type.h"
#include "Codec/Type/TypeExtension.h"

#include "AmqpTypes/AmqpVariable.h"

static inline void amqp_variable_initialize_from_type(amqp_variable_t *variable, amqp_type_t *type)
{
    variable->type = type;
    variable->size = type->position.size;
    amqp_block_header_initialize(&variable->block, 0, 0, 0);
    variable->data = 0;
}
static inline void amqp_variable_initialize(amqp_variable_t *variable, const uint8_t *data, size_t size)
{
    variable->type = 0;
    variable->size = size;
    amqp_block_header_initialize(&variable->block, size, size, 1);
    variable->data = data;
}
static inline void amqp_variable_initialize_as_null(amqp_variable_t *variable)
{
    variable->type = 0;
    variable->size = 0;
    amqp_block_header_initialize(&variable->block, 0, 0, 0);
    variable->data = 0;
}

extern int amqp_variable_to_bytes(amqp_variable_t *variable, uint8_t *buffer, size_t buffer_size);

extern int amqp_variable_compare(amqp_variable_t *lhs, amqp_variable_t *rhs);
extern int amqp_variable_compare_with_cstr(amqp_variable_t *lhs, const char *rhs);
extern int amqp_variable_compare_with_bytes(amqp_variable_t *lhs, const uint8_t *rhs, size_t size);
extern uint32_t amqp_variable_hash(amqp_variable_t *variable);

extern uint8_t *amqp_variable_clone_data(amqp_context_t *context, amqp_variable_t *source);

#ifdef __cplusplus
}
#endif
#endif
