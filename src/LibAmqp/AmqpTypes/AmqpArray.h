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

#ifndef LIBAMQP_AMQP_TYPES_AMQP_ARRAY_H
#define LIBAMQP_AMQP_TYPES_AMQP_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Codec/Type/Type.h"
#include "AmqpTypes/AmqpLeader.h"

struct amqp_array_t
{
    amqp_leader_t leader;
    amqp_type_t *type;
    size_t size;
    void **elements;
};

extern void amqp_array_initialize_as_null(amqp_context_t *context, amqp_array_t *array);

static inline
int amqp_array_is_null(amqp_array_t *array)
{
    return array->type == 0 && array->elements == 0;
}

extern void amqp_array_initialize(amqp_context_t *context, amqp_array_t *array, size_t size);
extern void amqp_array_initialize_from_type(amqp_context_t *context, amqp_array_t *array, amqp_type_t *type);
extern amqp_array_t *amqp_array_create(amqp_context_t *context, size_t size);
extern amqp_array_t *amqp_array_create_from_type(amqp_context_t *context, amqp_type_t *type);

static inline
void amqp_array_cleanup(amqp_context_t *context, amqp_array_t *array)
{
    amqp_type_cleanup(context, (amqp_amqp_type_t *) array);
}

static inline
size_t amqp_array_count(amqp_array_t *array)
{
    return array->size;
}

static inline
void amqp_array_set(amqp_array_t *array, size_t index, const void *p)
{
    assert(array->type == 0);
    array->elements[index] = (void *) p;
}

static inline
void *amqp_array_get(amqp_array_t *array, size_t index)
{
    assert(array->size > index);
    return array->type ? array->type->value.array.elements[index] : array->elements[index];
}

static inline
amqp_type_t *amqp_array_get_type(amqp_array_t *array, size_t index)
{
    assert(array->type);
    return (amqp_type_t *) amqp_array_get(array, index);
}

#ifdef __cplusplus
}
#endif
#endif
