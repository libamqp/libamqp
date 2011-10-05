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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_SYMBOL_H
#define LIBAMQP_AMQP_TYPES_AMQP_SYMBOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Context/Context.h"

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#ifndef LIBAMQP_AMQP_SYMBOL_T
#define LIBAMQP_AMQP_SYMBOL_T
    typedef struct amqp_symbol_t amqp_symbol_t;
#endif

struct amqp_symbol_t
{
    amqp_buffer_t *buffer;
    const unsigned char *reference;
    size_t size;
};

extern void amqp_symbol_initialize_reference(amqp_symbol_t *symbol, amqp_buffer_t *buffer, const unsigned char *reference, size_t n);
extern void amqp_symbol_cleanup(amqp_symbol_t *symbol);

extern int amqp_symbol_compare(amqp_symbol_t *lhs, amqp_symbol_t *rhs);
extern int amqp_symbol_hash(amqp_symbol_t *symbol);

extern int amqp_hash(const char *data, int len);

#ifdef __cplusplus
}
#endif
#endif