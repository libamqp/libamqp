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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_BINARY_H
#define LIBAMQP_AMQP_TYPES_AMQP_BINARY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "libamqp_common.h"
#include "Context/Context.h"

#ifndef LIBAMQP_AMQP_BINARY_TYPE_T
#define LIBAMQP_AMQP_BINARY_TYPE_T
typedef struct amqp_binary_t amqp_binary_t;
#endif

struct amqp_binary_t
{
    int on_heap;
};

extern void amqp_binary_initialize(amqp_context_t *context, amqp_binary_t *binary);
extern void amqp_binary_cleanup(amqp_context_t *context, amqp_binary_t *binary);

#ifdef __cplusplus
}
#endif
#endif
