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

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "Context/Context.h"
#include "Plugin/Sasl.h"
#include "AmqpTypes/AmqpSymbol.h"

#include "debug_helper.h"

struct amqp_sasl_plugin_node_t
{
    amqp_sasl_plugin_t *plugin;
    amqp_sasl_plugin_node_t *link;
};

void amqp_context_register_sasl_plugin(amqp_context_t *context, amqp_sasl_plugin_t *sasl_plugin)
{
    amqp_sasl_plugin_node_t *node = AMQP_MALLOC(context, amqp_sasl_plugin_node_t);
    node->plugin = sasl_plugin;
    node->link = context->reference.plugins.sasl_plugin_list;
    context->reference.plugins.sasl_plugin_list = node;
}

amqp_sasl_plugin_t *amqp_context_lookup_sasl_plugin(amqp_context_t *context, amqp_symbol_t *mechanism)
{
    amqp_sasl_plugin_node_t *list = context->reference.plugins.sasl_plugin_list;
    while (list)
    {
        if (amqp_symbol_compare_with_cstr(mechanism, list->plugin->mechanism_name) == 0)
        {
            return list->plugin;
        }
        list = list->link;
    }
    return 0;
}
void amqp_context_free_sasl_plugins(amqp_context_t *context)
{
    while (context->reference.plugins.sasl_plugin_list)
    {
        amqp_sasl_plugin_node_t *node = context->reference.plugins.sasl_plugin_list;
        context->reference.plugins.sasl_plugin_list = context->reference.plugins.sasl_plugin_list->link;
        
        node->plugin->plugin_cleanup_callback(context, node->plugin);
        AMQP_FREE(context, node);
    }
}
