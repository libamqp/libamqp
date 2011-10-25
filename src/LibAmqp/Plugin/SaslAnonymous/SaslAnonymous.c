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

#include "Context/Context.h"
#include "Plugin/SaslAnonymous/SaslAnonymous.h"
#include "debug_helper.h"

static void cleanup_plugin(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    AMQP_FREE(context, sasl_plugin);
}

static amqp_binary_t *initial_response_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    not_implemented(todo);
}

amqp_sasl_plugin_t *amqp_plugin_sasl_anonymous_create(amqp_context_t *context)
{
    amqp_sasl_plugin_t *result = AMQP_MALLOC(context, amqp_sasl_plugin_t);
    result->mechanism_name = "ANONYMOUS";
    result->plugin_cleanup_handler = cleanup_plugin;
    result->initial_response_handler = initial_response_handler;

    return result;
}
