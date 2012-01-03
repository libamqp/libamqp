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

#ifndef LIBAMQP_H
#define LIBAMQP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"

#define AMQP_DEFAULT_PORT           5672
#define AMQP_DEFAULT_SECURE_PORT    5671

#define AMQP_PROTOCOL_ID            0
#define AMQP_PROTOCOL_MAJOR         1
#define AMQP_PROTOCOL_MINOR         0
#define AMQP_PROTOCOL_REVISION      0

#define AMQP_TLS_PROTOCOL_ID        2
#define AMQP_TLS_PROTOCOL_MAJOR         1
#define AMQP_TLS_PROTOCOL_MINOR         0
#define AMQP_TLS_PROTOCOL_REVISION      0

#define AMQP_SASL_PROTOCOL_ID       3
#define AMQP_SASL_PROTOCOL_MAJOR         1
#define AMQP_SASL_PROTOCOL_MINOR         0
#define AMQP_SASL_PROTOCOL_REVISION      0

#define AMQP_MIN_MAX_FRAME_SIZE     512
#define AMQP_FRAME_HEADER_SIZE      8

#define AMQP_DEFAULT_MAX_FRAME_SIZE ((uint32_t) ((1 << 24) - 1))
#define AMQP_DEFAULT_CHANNEL_MAX ((uint16_t) 16)
#define AMQP_DEFAULT_IDLE_TIMEOUT ((uint32_t) 15 * 1000 * 2)

// TODO - external API signatures go here

#ifdef __cplusplus
}
#endif
#endif


