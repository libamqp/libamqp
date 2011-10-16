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

#ifndef LIBAMQP_TRANSPORT_SASL_SASL_MECHANISMS_H
#define LIBAMQP_TRANSPORT_SASL_SASL_MECHANISMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ev.h>

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_BUFFER_T_TYPE
#define LIBAMQP_BUFFER_T_TYPE
typedef struct amqp_buffer_t amqp_buffer_t;
#endif

#ifndef LIBAMQP_AMQP_TYPE_T
#define LIBAMQP_AMQP_TYPE_T
typedef struct amqp_type_t amqp_type_t;
#endif

typedef struct amqp_sasl_mechanism_t
{
    const char *name;
} amqp_sasl_mechanism_t;

extern amqp_type_t *amqp_sasl_mechanisms_encode(amqp_context_t *context, amqp_buffer_t *buffer);

#ifdef __cplusplus
}
#endif
#endif
