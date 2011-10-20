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
#include "debug_helper.h"

static amqp_sasl_mechanism_t supported_mechanisms[] =
{
    // TODO - assert that list is sorted
    { "PLAIN" },
    {0}
};

amqp_type_t *amqp_sasl_mechanisms_encode(amqp_context_t *context, amqp_buffer_t *buffer)
{
    amqp_type_t *result;
    int count = 0;
    int length = 0;
    int contains_long_name = false;

    while (supported_mechanisms[count].name)
    {
        int l = strlen(supported_mechanisms[count].name);
        length += l;
        contains_long_name = contains_long_name || l > 254;
        count++;
    }
    
    assert(count > 0);

    if (count == 1)
    {
        result = amqp_encode_symbol(context, buffer, supported_mechanisms[0].name);
    }
    else if ((length + count) < 253)
    {
        int i;
        result = amqp_encode_array_8(context, buffer);
        for (i = 0; i < count; i++)
        {
            amqp_encode_symbol_sym8(context, buffer, supported_mechanisms[i].name);
        }
        amqp_complete_type(context, buffer, result);
    }
    else
    {
        int i;
        result = amqp_encode_array_32(context, buffer);
        for (i = 0; i < count; i++)
        {
            if (contains_long_name)
            {
                amqp_encode_symbol_sym32(context, buffer, supported_mechanisms[i].name);
            }
            {
                amqp_encode_symbol_sym8(context, buffer, supported_mechanisms[i].name);
            }
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
