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

//#include "Context/Context.h"

#include "string.h"

#include "Context/Context.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Frame/Frame.h"
#include "Transport/Sasl/SaslMechanisms.h"
#include "Codec/Encode/Encode.h"
#include "Plugin/Sasl.h"
#include "debug_helper.h"


amqp_type_t *amqp_sasl_mechanisms_encode(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_sasl_plugin_node_t *iterator;

    amqp_type_t *result;
    int count = 0;
    int total_length = 0;
    int contains_long_name = false;
    amqp_sasl_plugin_t *plugin = amqp_context_first_sasl_plugin(context, &iterator);

    while (plugin)
    {
        int menchanism_name_length = strlen(plugin->mechanism_name);
        total_length += menchanism_name_length;
        contains_long_name = contains_long_name || menchanism_name_length > 254;
        count++;
        plugin = amqp_context_next_sasl_plugin(context, &iterator);
    }
    
    assert(count > 0);

    plugin = amqp_context_first_sasl_plugin(context, &iterator);

    if (count == 1)
    {
        result = amqp_encode_symbol(context, buffer, plugin->mechanism_name);
    }
    else if ((total_length + count) < 254)
    {
        result = amqp_encode_array_8(context, buffer);
        while (plugin)
        {
            amqp_encode_symbol_sym8(context, buffer, plugin->mechanism_name);
            plugin = amqp_context_next_sasl_plugin(context, &iterator);
        }
        amqp_complete_type(context, buffer, result);
    }
    else
    {
        result = amqp_encode_array_32(context, buffer);
        while (plugin)
        {
            if (contains_long_name)
            {
                amqp_encode_symbol_sym32(context, buffer, plugin->mechanism_name);
            }
            {
                amqp_encode_symbol_sym8(context, buffer, plugin->mechanism_name);
            }
            plugin = amqp_context_next_sasl_plugin(context, &iterator);
        }
        amqp_complete_type(context, buffer, result);
    }
    return result;
}

amqp_sasl_plugin_t *amqp_sasl_select_mechanism(amqp_connection_t *connection, amqp_multiple_symbol_t *multiple)
{
    int i;
    for (i = 0; i < amqp_multiple_symbol_size(multiple); i++)
    {
        amqp_sasl_plugin_t *result = amqp_context_lookup_sasl_plugin(connection->context, amqp_multiple_symbol_get(multiple, i));
        if (result)
        {
            return result;
        }
    }
    return 0;
}
