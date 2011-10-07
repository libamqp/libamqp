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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_LEADER_H
#define LIBAMQP_AMQP_TYPES_AMQP_LEADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "libamqp_common.h"

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
    typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#ifndef LIBAMQP_AMQP_BINARY_TYPE_T
#define LIBAMQP_AMQP_BINARY_TYPE_T
typedef struct amqp_binary_t amqp_binary_t;
#endif

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_ENTRY_T
#define LIBAMQP_AMQP_ENTRY_T
typedef struct amqp_entry_t amqp_entry_t;
#endif

#ifndef LIBAMQP_AMQP_LIST_T
#define LIBAMQP_AMQP_LIST_T
    typedef struct amqp_list_t amqp_list_t;
#endif

#ifndef LIBAMQP_AMQP_MAP_TYPE_T
#define LIBAMQP_AMQP_MAP_TYPE_T
typedef struct amqp_map_t amqp_map_t;
#endif

#ifndef LIBAMQP_AMQP_MULTIPLE_SYMBOL_T
#define LIBAMQP_AMQP_MULTIPLE_SYMBOL_T
    typedef struct amqp_multiple_symbol_t amqp_multiple_symbol_t;
#endif

#ifndef LIBAMQP_AMQP_STRING_TYPE_T
#define LIBAMQP_AMQP_STRING_TYPE_T
typedef struct amqp_string_t amqp_string_t;
#endif

#ifndef LIBAMQP_AMQP_SYMBOL_T
#define LIBAMQP_AMQP_SYMBOL_T
    typedef struct amqp_symbol_t amqp_symbol_t;
#endif

#ifndef LIBAMQP_AMQP_TYPES_T
#define LIBAMQP_AMQP_TYPES_T
    typedef union amqp_types_t amqp_types_t;
    typedef void (*amqp_dtor_t)(amqp_context_t *context, amqp_types_t *type);
#endif

typedef struct amqp_leader_t
{
    amqp_dtor_t dtor;
} amqp_leader_t;


extern uint32_t amqp_hash(const void *data, int len);

#ifdef __cplusplus
}
#endif
#endif
