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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_MULTIPLE_H
#define LIBAMQP_AMQP_TYPES_AMQP_MULTIPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AmqpTypes/AmqpLeader.h"
#include "AmqpTypes/AmqpSymbol.h"

#define AMQP_MULTIPLE_DEFAULT_SIZE  8

struct amqp_multiple_symbol_t
{
    amqp_leader_t leader;
    amqp_type_t *type;
    int size;       // TODO - rename to count?
    amqp_symbol_t *symbols;
};

static inline
void amqp_multiple_symbol_cleanup(amqp_context_t *context, amqp_multiple_symbol_t *multiple)
{
    amqp_type_cleanup(context, (amqp_amqp_type_t *) multiple);
}

static inline
int amqp_multiple_symbol_size(amqp_multiple_symbol_t *multiple)
{
    return multiple->size;
}

static inline
amqp_symbol_t *amqp_multiple_symbol_get(amqp_multiple_symbol_t *multiple, int element)
{
    assert(multiple != 0);

    return (multiple->size > 0 && element < multiple->size) ? &multiple->symbols[element] : 0;
}

extern void amqp_multiple_symbol_initialize_as_null(amqp_context_t *context, amqp_multiple_symbol_t *multiple);
extern int amqp_multiple_symbol_initialize(amqp_context_t *context, amqp_multiple_symbol_t *multiple, amqp_type_t *type);
extern amqp_multiple_symbol_t *amqp_multiple_symbol_create(amqp_context_t *context, amqp_type_t *type);
static inline
amqp_multiple_symbol_t *amqp_multiple_symbol_create_from_type(amqp_context_t *context, amqp_type_t *type)
{
    return amqp_multiple_symbol_create(context, type);
}
extern void amqp_multiple_symbol_cleanup(amqp_context_t *context, amqp_multiple_symbol_t *multiple);
extern int amqp_multiple_symbol_to_char_bytes(amqp_multiple_symbol_t *multiple, const char *spacer, char *buffer, size_t buffer_size);
extern int amqp_multiple_symbol_total_length(amqp_multiple_symbol_t *multiple);
#ifdef __cplusplus
}
#endif
#endif
