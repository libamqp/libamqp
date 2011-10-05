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

#ifndef LIBAMQP_CODEC_TYPE_TYPE_EXTENSION_H
#define LIBAMQP_CODEC_TYPE_TYPE_EXTENSION_H

#include "Codec/Type/Type.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline
bool amqp_type_convert_ok(amqp_type_t *type)
{
    return type->flags.convert_failed == 0;
}

static inline
amqp_type_t *amqp_type_convert_has_failed(amqp_type_t *type)
{
    type->flags.convert_failed = 1;
    return type;
}

static inline
amqp_type_t *amqp_type_get_descriptor(amqp_type_t *type)
{
    return amqp_type_is_described(type) ? type->value.described.elements[0] : amqp_type_convert_has_failed(type);
}

static inline
amqp_type_t *amqp_type_get_described(amqp_type_t *type)
{
    return amqp_type_is_described(type) ? type->value.described.elements[1] : amqp_type_convert_has_failed(type);
}


#ifdef __cplusplus
}
#endif
#endif


