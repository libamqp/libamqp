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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_ENTRY_H
#define LIBAMQP_AMQP_TYPES_AMQP_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AmqpTypes/AmqpLeader.h"

struct amqp_entry_t
{
    amqp_leader_t leader;
    struct {
        amqp_entry_t *next;
        amqp_entry_t **prev;
    } collision_list;
    struct {
        amqp_entry_t *next;
        amqp_entry_t **prev;
    } entry_list;
    const void *key;
    const void *data;
};

typedef uint32_t (*amqp_hash_fn_t)(const void *key);
typedef int (*amqp_compare_fn_t)(const void *lhs, const void *rhs);
typedef void (*amqp_free_callback_t)(amqp_context_t *context, const void *key, const void *data);

#ifdef __cplusplus
}
#endif
#endif
