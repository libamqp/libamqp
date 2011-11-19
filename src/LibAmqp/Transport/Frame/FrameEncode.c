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
#include "Transport/Frame/FrameEncode.h"
#include "Transport/Connection/Connection.h"
#include "Codec/Encode/Encode.h"
#include "AmqpTypes/AmqpTypes.h"
#include "Transport/Sasl/SaslMechanisms.h"

#include "debug_helper.h"

#define DEFAULT_TYPE_SPECIFIC_FIELD   0
#define DEFAULT_FIELDS_ENCODER_ARG   0
#define CHANNEL_ZERO   0

typedef int (*amqp_frame_encoder_t)(amqp_connection_t *connection, amqp_buffer_t *buffer, void *encoder_arg);

static inline
int encode_frame_list(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_frame_encoder_t fields_encoder, void *fields_encoder_arg)
{
    int rc;
    amqp_type_t *list = amqp_encode_list_8(connection->context, buffer);
    rc = fields_encoder(connection, buffer, fields_encoder_arg);
    amqp_complete_type(connection->context, buffer, list);
    return rc;
}

static inline
int encode_performative(amqp_connection_t *connection, amqp_buffer_t *buffer, uint64_t id, amqp_frame_encoder_t fields_encoder, void *encoder_arg)
{
    int rc;
    amqp_context_t *context = connection->context;
    amqp_type_t *frame = amqp_start_encode_described_type(context, buffer);
    amqp_encode_ulong(context, buffer, id);
    rc = encode_frame_list(connection, buffer, fields_encoder, encoder_arg);
    amqp_complete_type(context, buffer, frame);
    amqp_deallocate_type(context, frame);
    return rc;
}

static
int amqp_encode_frame(amqp_connection_t *connection, amqp_buffer_t *buffer, uint64_t id, uint8_t frame_type, uint16_t frame_type_specific, amqp_frame_encoder_t fields_encoder, void *encoder_arg)
{
    size_t performativce_offset, frame_size;
    int rc;

    amqp_buffer_reset(buffer);
    amqp_buffer_advance_write_point(buffer, 8);
    performativce_offset = amqp_buffer_write_point(buffer);

    rc = encode_performative(connection, buffer, id, fields_encoder, encoder_arg);

    frame_size = amqp_buffer_size(buffer);
    amqp_buffer_set_write_index(buffer, 0);
    amqp_buffer_write_uint32(buffer, frame_size);
    amqp_buffer_write_uint8(buffer, performativce_offset / AMQP_FRAME_HEADER_DATA_OFFSET_MULTIPLIER);
    amqp_buffer_write_uint8(buffer, frame_type);
    amqp_buffer_write_uint16(buffer, frame_type_specific);
    amqp_buffer_set_write_index(buffer, frame_size);

    return rc;
}

static int amqp_sasl_mechanisms_field_encoder(amqp_connection_t *connection, amqp_buffer_t *buffer, void *ignored)
{
    if (amqp_context_sasl_plugin_count(connection->context) == 0)
    {
        amqp_connection_failed(connection, AMQP_ERROR_SASL_PLUGINS_REGISTERED, AMQP_CONNECTION_SASL_ERROR, "Cannot construct SASL Mechanisms list. No SASL plugins registered.");
        return 0;
    }
    // TODO use a differet plugin list for server and client.
    amqp_sasl_mechanisms_encode(connection->context, buffer);
    return 1;
}

int amqp_encode_sasl_mechanisms_frame(amqp_connection_t *connection, amqp_buffer_t *buffer)
{
    assert(connection && buffer);
    return amqp_encode_frame(connection, buffer, amqp_sasl_mechanisms_list_descriptor, AMQP_SASL_FRAME_TYPE, DEFAULT_TYPE_SPECIFIC_FIELD, amqp_sasl_mechanisms_field_encoder, DEFAULT_FIELDS_ENCODER_ARG);
}

static int amqp_sasl_init_field_encoder(amqp_connection_t *connection, amqp_buffer_t *buffer, void *arg)
{
    amqp_sasl_plugin_t *sasl_plugin = (amqp_sasl_plugin_t *) arg;

    assert(sasl_plugin && sasl_plugin->mechanism_name);

    if (connection->socket.hostname == 0)
    {
        amqp_connection_failed(connection, AMQP_ERROR_NO_HOST_NAME, AMQP_CONNECTION_SASL_ERROR, "Cannot initialize SASL Init frame. No hostname provided.");
        return 0;
    }

    amqp_encode_symbol(connection->context, buffer, sasl_plugin->mechanism_name);
    amqp_sasl_plugin_initial_response_encode(connection->context, sasl_plugin, &connection->sasl.identity_hooks, buffer);
    amqp_encode_string(connection->context, buffer, amqp_connection_target_host(connection));
    return 1;
}

int  amqp_encode_sasl_init_frame(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_sasl_plugin_t *sasl_plugin)
{
    return amqp_encode_frame(connection, buffer, amqp_sasl_init_list_descriptor, AMQP_SASL_FRAME_TYPE, DEFAULT_TYPE_SPECIFIC_FIELD, amqp_sasl_init_field_encoder, sasl_plugin);
}

static
int amqp_sasl_outcome_field_encoder(amqp_connection_t *connection, amqp_buffer_t *buffer, void *arg)
{
    amqp_sasl_plugin_t *plugin = (amqp_sasl_plugin_t *) arg;
    amqp_encode_ubyte(connection->context, buffer, plugin ? plugin->outcome : amqp_sasl_code_system_error);
    return 1;
}
// TODO - this is only good when response is the result of init_response, not challenge response
int  amqp_encode_sasl_outcome_frame(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_sasl_plugin_t *sasl_plugin)
{
    return amqp_encode_frame(connection, buffer, amqp_sasl_outcome_list_descriptor, AMQP_SASL_FRAME_TYPE, DEFAULT_TYPE_SPECIFIC_FIELD, amqp_sasl_outcome_field_encoder, sasl_plugin);
}

//struct challenge_encoder_args
//{
//    amqp_sasl_plugin_t *plugin;
//    amqp_security_sasl_init_t *init_response;
//};
//static int amqp_sasl_challenge_field_encoder(amqp_connection_t *connection, amqp_buffer_t *buffer, void *arg)
//{
//    struct challenge_encoder_args *challenge_encoder_args = (struct challenge_encoder_args *) arg;
//
//    amqp_sasl_plugin_challenge(connection->context, challenge_encoder_args->plugin, buffer, &connection->sasl.identity_hooks, challenge_encoder_args->init_response);
//    return 1;
//}
int  amqp_encode_sasl_challenge_frame(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_sasl_plugin_t *sasl_plugin)
{
    not_implemented(todo);
//    struct challenge_encoder_args args = {.plugin = sasl_plugin, .init_response = init_response};
//    return amqp_encode_frame(connection, buffer, amqp_sasl_challenge_list_descriptor, AMQP_SASL_FRAME_TYPE, DEFAULT_TYPE_SPECIFIC_FIELD, amqp_sasl_challenge_field_encoder, &args);
}

static
int amqp_open_field_encoder(amqp_connection_t *connection, amqp_buffer_t *buffer, void *arg)
{
    amqp_encode_string(connection->context, buffer, connection->amqp.connection.local_container_id);
    if (amqp_connection_is_client(connection))
    {
        SOUTS("CLIENT");
        amqp_encode_string(connection->context, buffer, amqp_connection_target_host(connection));
    }
    else
    {
        SOUTS("SERVER");
        amqp_encode_null(connection->context, buffer);
    }
    amqp_encode_uint(connection->context, buffer, connection->amqp.connection.limits.max_frame_size);
    amqp_encode_ushort(connection->context, buffer, connection->amqp.connection.limits.channel_max);
    amqp_encode_uint(connection->context, buffer, connection->amqp.connection.limits.idle_time_out);

    return 1;
}

int amqp_encode_amqp_open(amqp_connection_t *connection, amqp_buffer_t *buffer)
{
    return amqp_encode_frame(connection, buffer, amqp_open_list_descriptor, AMQP_FRAME_TYPE, CHANNEL_ZERO, amqp_open_field_encoder, 0);
}
