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

#include "Context/Context.h"
#include "Codec/Encode/Encode.h"

#include "Plugin/SaslAnonymous/SaslAnonymous.h"
#include "debug_helper.h"

static void cleanup_instance_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    assert(sasl_plugin != 0);
    AMQP_FREE(context, sasl_plugin);
}

static
amqp_type_t *initial_response_encoder(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_sasl_identity_t *identity_hooks, amqp_buffer_t *buffer)
{
    const char *email;

    assert(identity_hooks->email);

    email = identity_hooks->email(context);

    return amqp_encode_binary(context, buffer, (unsigned char *) email, strlen(email));
}

static
int initial_response_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_sasl_identity_t *identity_hooks, amqp_security_sasl_init_t *init_response)
{
    not_implemented(todo);
}

static amqp_sasl_plugin_t *instance_create_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    amqp_sasl_plugin_t *result = amqp_sasl_plugin_base_instance_create(context, sasl_plugin);

    result->essence.instance.cleanup_instance_handler = cleanup_instance_handler;
    result->essence.instance.initial_response_encoder = initial_response_encoder;
    result->essence.instance.initial_response_handler = initial_response_handler;

    return result;
}

amqp_sasl_plugin_t *amqp_plugin_sasl_anonymous_create(amqp_context_t *context)
{
    return amqp_sasl_plugin_base_create(context, "ANONYMOUS", instance_create_handler);
}
