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

//#include "Context/Context.h"

#include "string.h"

#include "Context/Context.h"
#include "Transport/Connection/Connection.h"
#include "Transport/Frame/Frame.h"
#include "Transport/Sasl/SaslMechanisms.h"
#include "Transport/Frame/FrameEncode.h"
#include "Codec/Encode/Encode.h"
#include "Plugin/Sasl.h"
#include "AmqpTypes/AmqpSymbol.h"

#include "Transport/Sasl/ServerSasl.h"

#include "debug_helper.h"


int amqp_sasl_send_mechanisms_frame(amqp_connection_t *connection)
{
    amqp_buffer_t *buffer = amqp_encode_sasl_mechanisms_frame(connection);
    amqp_connection_write_buffer(connection, buffer);
    return buffer != 0;
}

static
void generate_plugin_list(amqp_context_t *context, uint8_t *buffer, size_t buffer_size)
{
    amqp_sasl_plugin_node_t *iterator;
    amqp_sasl_plugin_t *plugin = amqp_context_first_sasl_plugin(context, &iterator);
    char *pad = "";

    while (plugin)
    {
        int n = snprintf((char *) buffer, buffer_size, "%s%s", pad, plugin->mechanism_name);
        buffer += n;
        buffer_size -= n;
        pad = ", ";
        plugin = amqp_context_next_sasl_plugin(context, &iterator);
    }
}

static
void unsupported_sasl_mechanism_error(amqp_connection_t *connection, amqp_symbol_t *mechanism)
{
    const int buffer_size = 256;
    uint8_t *buffer = amqp_allocate_print_buffer(connection->context, buffer_size);
    unsigned char mechanism_name[65];
    int n = amqp_symbol_to_bytes(mechanism, mechanism_name, 64);
    mechanism_name[n] = '\0';
    generate_plugin_list(connection->context, buffer, buffer_size);
    amqp_connection_failed(connection, AMQP_ERROR_UNSUPPORTED_SASL_MECHANISM, AMQP_CONNECTION_SASL_ERROR,
            "SASL Init frame for machanism \"%s\" is not supported by broker. Broker supports: %s", mechanism_name, buffer);
    amqp_deallocate_print_buffer(connection->context, buffer);
}

/*
static
int amqp_encode_sasl_challenge(amqp_connection_t *connection, amqp_sasl_plugin_t *plugin, amqp_security_sasl_init_t *init_response)
{
    amqp_buffer_reset(connection->buffer.write);
    return amqp_encode_sasl_challenge_frame(connection, connection->buffer.write, plugin, init_response);
}

*/

static
int amqp_sasl_send_sasl_outcome(amqp_connection_t *connection, amqp_sasl_plugin_t *plugin)
{
    amqp_buffer_t *buffer = amqp_encode_sasl_outcome_frame(connection, plugin);
    amqp_connection_write_buffer(connection, buffer);
    return buffer != 0;
}

/*
    Got SASL Init response from the client. SASL Plugin may send a Challenge or Outcome reply
*/
int amqp_sasl_process_init_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    int rc;
    amqp_symbol_t *mechanism = &frame->frames.sasl.init.mechanism;
    amqp_sasl_plugin_t *plugin = amqp_context_lookup_sasl_plugin(connection->context, mechanism);

    if (plugin == 0)
    {
        unsupported_sasl_mechanism_error(connection, mechanism);
        // TODO - consider sending a SASL Outcome frame rather than pulling the plug
        return amqp_plugin_handler_failed;
    }

    connection->sasl.plugin = plugin;
    rc = amqp_sasl_plugin_initial_response_handle(connection->context, plugin, &connection->sasl.identity_hooks, &frame->frames.sasl.init);
    switch (rc)
    {
    case amqp_plugin_handler_outcome_accepted:
        if (!amqp_sasl_send_sasl_outcome(connection, plugin))
        {
            return amqp_sasl_frame_process_error;
        }
        break;

    case amqp_plugin_handler_challenge:
        not_implemented(todo);

    case amqp_plugin_handler_failed:
        not_implemented(todo);

    case amqp_plugin_handler_outcome_rejected:
        not_implemented(todo);

    default:
        abort();
    }
    return rc;
}

int amqp_sasl_process_challenge_response_frame(amqp_connection_t *connection, amqp_frame_t *frame)
{
    not_implemented(todo);
}
