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

#ifndef LIBAMQP_TYPE_H
#define LIBAMQP_TYPE_H

#include "libamqp_common.h"
#include "Codec/Type/TypeMetaData.h"
#include "Context/Context.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIBAMQP_AMQP_TYPE_T
#define LIBAMQP_AMQP_TYPE_T
typedef struct amqp_type_t amqp_type_t;
#endif

#ifndef LIBAMQP_CONTEXT_TYPE
#define LIBAMQP_CONTEXT_TYPE
typedef struct amqp_context_t amqp_context_t;
#endif

typedef struct amqp_buffer_position_t
{
    size_t index;
    size_t size;
} amqp_buffer_position_t;

typedef struct {
    unsigned int is_array:1;
    unsigned int is_list:1;
    unsigned int is_map:1;
    /**
        A compound type with two elements; one defining the type's descriptor
        and the second defining the type's structure.
     */
    unsigned int is_described:1;
} amqp_type_type_flags_t;

typedef struct {
    unsigned int is_null:1;
    unsigned int is_invalid:1;
    unsigned int is_encoded:1;
    unsigned int is_incomplete:1;
    unsigned int is_contained:1;
    unsigned int is_descriptor:1;
    unsigned int has_descriptor:1;
    union {
        unsigned int is_compound;
        amqp_type_type_flags_t flags;
    } structure;
} amqp_type_flags_t;

struct amqp_type_t
{
    amqp_context_t *context;

    int format_code;
    int extension_type_code;
    amqp_type_meta_data_t *meta_data;

    amqp_buffer_position_t position;
    amqp_type_flags_t flags;

    union {
        amqp_eight_byte_t b8;
        amqp_four_byte_t b4;
        amqp_two_byte_t b2;
        amqp_one_byte_t b1;
        const unsigned char *uuid;
        struct {
            size_t count;
            amqp_type_t **elements;
            amqp_type_t *saved_container;
        } compound;
        struct {
            size_t count;
            amqp_type_t **elements;
        } array;
        struct {
            size_t count;
            amqp_type_t **elements;
        } list;
        struct {
            size_t count;
            amqp_type_t **entries;
        } map;
        struct {
            size_t count;
            amqp_type_t **elements;
        } described;
    } value;
    int invalid_cause ;
};

extern void amqp_type_initialize_pool(amqp_memory_pool_t *pool);

extern amqp_type_t **amqp_allocate_amqp_type_t_array(size_t count);
extern void amqp_deallocate_amqp_type_t_array(amqp_memory_pool_t *pool, amqp_type_t **array, size_t count);

extern amqp_type_t **amqp_realloc_amqp_type_t_array(amqp_type_t **elements, size_t count);
extern int amqp_type_match(amqp_type_t *lhs, amqp_type_t *rhs);
extern void amqp_mark_type_invalid(amqp_type_t *type, int cause);
extern void amqp_describe_type(char *buffer, size_t size, amqp_type_t *type);
extern amqp_type_t *amqp_allocate_type(amqp_context_t *context);

static inline
amqp_buffer_t *amqp_type_convert_buffer(amqp_type_t *type)
{
    return type->flags.is_encoded ? type->context->encode.buffer : type->context->decode.buffer;
}

static inline
bool amqp_type_is_compound(amqp_type_t *type)
{
    return type->flags.structure.is_compound != 0;
}

static inline
bool amqp_type_is_map(amqp_type_t *type)
{
    return type->flags.structure.flags.is_map != 0;
}

static inline
bool amqp_type_is_array(amqp_type_t *type)
{
    return type->flags.structure.flags.is_array != 0;
}

static inline
bool amqp_type_is_list(amqp_type_t *type)
{
    return type->flags.structure.flags.is_list != 0;
}

static inline
bool amqp_type_is_valid(amqp_type_t *type)
{
    return type->flags.is_invalid == 0 && type->flags.is_incomplete == 0;
}

static inline
bool amqp_type_is_contained(amqp_type_t *type)
{
    return type->flags.is_contained != 0;
}

static inline
bool amqp_type_is_described(amqp_type_t *type)
{
    return type->flags.structure.flags.is_described != 0;
}

static inline
bool amqp_type_has_descriptor(amqp_type_t *type)
{
    return type->flags.has_descriptor != 0;
}

static inline
bool amqp_type_is_descriptor(amqp_type_t *type)
{
    return type->flags.is_descriptor != 0;
}


#ifdef __cplusplus
}
#endif
#endif

