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

#include "Plugin/Sasl.h"
#include "Context/Context.h"

void amqp_sasl_plugin_cleanup(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    if (sasl_plugin)
    {
        assert(sasl_plugin->essence.plugin.cleanup_plugin_handler);
        sasl_plugin->essence.plugin.cleanup_plugin_handler(context, sasl_plugin);
        sasl_plugin->essence.plugin.cleanup_plugin_handler = 0;
    }
}

void amqp_sasl_plugin_instance_cleanup(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    if (sasl_plugin)
    {
        assert(sasl_plugin->essence.instance.cleanup_instance_handler);
        sasl_plugin->essence.instance.cleanup_instance_handler(context, sasl_plugin);
        sasl_plugin->essence.instance.cleanup_instance_handler = 0;
    }
}

amqp_sasl_plugin_t *amqp_sasl_plugin_instance_create(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    assert(sasl_plugin);
    assert(sasl_plugin->essence.plugin.instance_create_handler);
    return sasl_plugin->essence.plugin.instance_create_handler(context, sasl_plugin);
}

amqp_type_t *amqp_sasl_plugin_initial_response(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_buffer_t *buffer, amqp_sasl_identity_t *identity_hooks)
{
    assert(sasl_plugin && sasl_plugin->essence.instance.initial_response_handler);
    return sasl_plugin->essence.instance.initial_response_handler(context, sasl_plugin, buffer, identity_hooks);
}

amqp_type_t *amqp_sasl_plugin_challenge_response(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_binary_t *challenge, amqp_buffer_t *buffer, amqp_sasl_identity_t *identity_hooks)
{
    assert(sasl_plugin && sasl_plugin->essence.instance.challenge_response_handler);
    return sasl_plugin->essence.instance.challenge_response_handler(context, sasl_plugin, challenge, buffer, identity_hooks);
}

//
static void base_instance_cleanup_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    assert(sasl_plugin != 0);
    assert(sasl_plugin->essence.instance.instance_state == 0);
    AMQP_FREE(context, sasl_plugin);
}

static amqp_type_t *initial_response_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_buffer_t *buffer, amqp_sasl_identity_t *identity_hooks)
{
    not_implemented(todo);
}

static amqp_type_t *challenge_response_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin, amqp_binary_t *challenge, amqp_buffer_t *buffer, amqp_sasl_identity_t *identity_hooks)
{
    not_implemented(todo);
}

amqp_sasl_plugin_t *amqp_sasl_plugin_base_instance_create(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    amqp_sasl_plugin_t *result = AMQP_MALLOC(context, amqp_sasl_plugin_t);
    result->mechanism_name = sasl_plugin->mechanism_name;
    result->essence.instance.cleanup_instance_handler = base_instance_cleanup_handler;
    result->essence.instance.initial_response_handler = initial_response_handler;
    result->essence.instance.challenge_response_handler = challenge_response_handler;
    result->essence.instance.instance_state = 0;
    return result;
}

static void base_plugin_cleanup_handler(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    assert(sasl_plugin != 0);
    assert(sasl_plugin->essence.plugin.plugin_state == 0);
    AMQP_FREE(context, sasl_plugin);
}

// Initializing what is analogous to a class instance
amqp_sasl_plugin_t *amqp_sasl_plugin_base_create(amqp_context_t *context, const char *name, amqp_sasl_plugin_instance_create_handler instance_create_handler)
{
    amqp_sasl_plugin_t *result = AMQP_MALLOC(context, amqp_sasl_plugin_t);
    result->mechanism_name = name;
    result->essence.plugin.cleanup_plugin_handler = base_plugin_cleanup_handler;
    result->essence.plugin.instance_create_handler = instance_create_handler;
    result->essence.plugin.plugin_state = 0;
    return result;
}
