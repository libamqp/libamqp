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

#ifndef LIBAMQP_PLUGIN_SASL_H
#define LIBAMQP_PLUGIN_SASL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "AmqpTypes/AmqpBinary.h"

#ifndef LIBAMQP_AMQP_CONTEXT_TYPE_T
#define LIBAMQP_AMQP_CONTEXT_TYPE_T
    typedef struct amqp_context_t amqp_context_t;
#endif

#ifndef LIBAMQP_AMQP_SASL_PLUGIN_TYPE_T
#define LIBAMQP_AMQP_SASL_PLUGIN_TYPE_T
typedef struct amqp_sasl_plugin_t amqp_sasl_plugin_t;
#endif

typedef void (*amqp_sasl_plugin_cleanup_handler)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin);
typedef amqp_binary_t *(*amqp_sasl_plugin_initial_response_handler)(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin);

struct amqp_sasl_plugin_t
{
    const char *mechanism_name;   
    amqp_sasl_plugin_cleanup_handler plugin_cleanup_handler;
    amqp_sasl_plugin_initial_response_handler initial_response_handler;
};

#ifdef __cplusplus
}
#endif
#endif
