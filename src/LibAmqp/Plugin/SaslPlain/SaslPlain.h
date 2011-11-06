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

#ifndef LIBAMQP_PLUGIN_SASLPLAIN_SASLPLAIN_H
#define LIBAMQP_PLUGIN_SASLPLAIN_SASLPLAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "libamqp_common.h"
#include "Plugin/Sasl.h"

extern amqp_sasl_plugin_t *amqp_plugin_sasl_plain_create(amqp_context_t *context);

extern amqp_sasl_plugin_t *amqp_plugin_sasl_plain_instance_create(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin);

#ifdef __cplusplus
}
#endif
#endif
