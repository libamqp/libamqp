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
#include "Transport/Frame/FrameEncode.h"
#include "Codec/Encode/Encode.h"
#include "Plugin/Sasl.h"

#include "Transport/Sasl/ClientSasl.h"

#include "debug_helper.h"

//static
void amqp_encode_sasl_init_response(amqp_connection_t *connection, amqp_sasl_plugin_t *plugin)
{
    amqp_buffer_reset(connection->buffer.write);
    amqp_encode_sasl_init_frame(connection, connection->buffer.write, plugin);
}

static void no_supported_sasl_mechanism_error(amqp_connection_t *connection, amqp_multiple_symbol_t *multiple)
{
    // TODO - add details of available SASL plugins.
    int space = amqp_multiple_symbol_total_length(multiple) + (amqp_multiple_symbol_size(multiple) - 1) * 2 + 1;
    uint8_t *buffer = amqp_allocate_print_buffer(connection->context, space);
    amqp_multiple_symbol_to_char_bytes(multiple, ", ", buffer, space);
    amqp_connection_failed(connection, AMQP_ERROR_NO_SASL_MECHANISM, AMQP_CONNECTION_SASL_ERROR, "No SASL plugin matches broker mechanism list. Broker supports: %s", buffer);
    amqp_deallocate_print_buffer(connection->context, buffer);
}

int amqp_sasl_process_mechanisms_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    // client has received the mechanisms from from the broker.
    // Find the first supported mechanism and get it to initialise the sasl init frame
    amqp_multiple_symbol_t *multiple = &frame->frames.sasl.mechanisms.sasl_server_mechanisms;
    amqp_sasl_plugin_t *plugin = amqp_sasl_select_mechanism(connection, multiple);
    if (plugin)
    {
        amqp_encode_sasl_init_response(connection, plugin);
        amqp_sasl_plugin_instance_cleanup(connection->context, plugin);
        return true;
    }
    else
    {
        no_supported_sasl_mechanism_error(connection, multiple);
        return false;
    }
}