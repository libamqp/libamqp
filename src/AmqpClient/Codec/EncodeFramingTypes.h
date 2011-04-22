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

#ifndef STORMMQ_AMQP_ENCODE_FRAMING_TYPES_H
#define STORMMQ_AMQP_ENCODE_FRAMING_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif

#include "Codec/Encode.h"
#include "Type/TypeEncodeMetaData.h"

static inline
amqp_type_t *amqp_encode_seconds(amqp_context_t *context, uint32_t seconds)
{
    return amqp_encode_uint(context, seconds);
}

amqp_type_t *amqp_encode_multiple_true_va(amqp_context_t *context, amqp_type_encode_meta_data_t *mata_data, ...);
#define amqp_encode_multiple_true(c, ...) amqp_encode_multiple_true_va(c, __VA_ARGS__, 0);

#ifdef __cplusplus
}
#endif
#endif


