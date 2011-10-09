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

#ifndef LIBAMQP_TRANSPORT_FRAME_TYPE_INIT_H
#define LIBAMQP_TRANSPORT_FRAME_TYPE_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Context/Context.h"
#include "Codec/Type/TypeExtension.h"

#include "AmqpTypes/AmqpTypes.h"

#include "Amqp/AmqpSecurity.h"
extern int amqp_type_to_symbol(amqp_context_t *context, amqp_symbol_t *symbol, amqp_type_t *type);
extern int amqp_type_to_multiple_symbol(amqp_context_t *context, amqp_frame_sasl_mechanisms_t *sasl_mechanisms, amqp_buffer_t *buffer, amqp_type_t *type);

#ifdef __cplusplus
}
#endif
#endif
