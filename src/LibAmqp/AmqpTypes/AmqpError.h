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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_ERROR_H
#define LIBAMQP_AMQP_TYPES_AMQP_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AmqpTypes/AmqpLeader.h"
#include "Codec/Type/TypeExtension.h"
#include "Amqp/AmqpDefinitions.h"

static inline
int amqp_error_is_defined(amqp_error_t *error)
{
    return error && !amqp_symbol_is_null(&error->condition);
}

#ifdef __cplusplus
}
#endif
#endif
