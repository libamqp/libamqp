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
#include "Transport/Frame/TypeInit.h"

#include "debug_helper.h"

// TODO - remove and move into typeextension
int amqp_type_to_symbol(amqp_context_t *context, amqp_symbol_t *symbol, amqp_type_t *type)
{
    // TODO - deal with case where symbol straddles buffer fragments - not an issue for performative frames as they should fit in the smallest buffer.
    amqp_symbol_initialize_from_type(context, symbol, type);
    return true;
}

int amqp_type_to_multiple_symbol(amqp_context_t *context, amqp_frame_sasl_mechanisms_t *sasl_mechanisms, amqp_buffer_t *buffer, amqp_type_t *type)
{
    if (amqp_type_is_null(type))
    {
        amqp_multiple_symbol_initialize(context, &sasl_mechanisms->sasl_server_mechanisms, 0);
    }

    if (amqp_type_is_symbol(type))
    {
        amqp_multiple_symbol_initialize(context, &sasl_mechanisms->sasl_server_mechanisms, 1);
        // TODO - add the single symbol to the multiple
        // create symbol, add symbol to multiple
    }

    if (amqp_type_is_array(type))
    {
        // create multiple and add each element of aray
    }

    return false;
}
