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

typedef void (*amqp_frame_encoder_t)(amqp_connection_t *connection, amqp_buffer_t *buffer, void *encoder_arg);

static inline
void encode_frame_list(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_frame_encoder_t fields_encoder, void *fields_encoder_arg)
{
    amqp_type_t *list = amqp_encode_list_8(connection->context, buffer);
    fields_encoder(connection, buffer, fields_encoder_arg);
    amqp_complete_type(connection->context, buffer, list);
}

static inline
void encode_performative(amqp_connection_t *connection, amqp_buffer_t *buffer, uint64_t id, amqp_frame_encoder_t fields_encoder, void *encoder_arg)
{
    amqp_context_t *context = connection->context;
    amqp_type_t *frame = amqp_start_encode_described_type(context, buffer);
    amqp_encode_ulong(context, buffer, id);
    encode_frame_list(connection, buffer, fields_encoder, encoder_arg);
    amqp_complete_type(context, buffer, frame);
    amqp_deallocate_type(context, frame);
}

static
void amqp_encode_frame(amqp_connection_t *connection, amqp_buffer_t *buffer, uint64_t id, uint8_t frame_type, uint16_t frame_type_specific, amqp_frame_encoder_t fields_encoder, void *encoder_arg)
{
    size_t performativce_offset, frame_size;;

    amqp_buffer_advance_write_point(buffer, 8);
    performativce_offset = amqp_buffer_write_point(buffer);

    encode_performative(connection, buffer, id, fields_encoder, encoder_arg);

    frame_size = amqp_buffer_size(buffer);
    amqp_buffer_set_write_index(buffer, 0);
    amqp_buffer_write_uint32(buffer, frame_size);
    amqp_buffer_write_uint8(buffer, performativce_offset / AMQP_FRAME_HEADER_DATA_OFFSET_MULTIPLIER);
    amqp_buffer_write_uint8(buffer, frame_type);
    amqp_buffer_write_uint16(buffer, frame_type_specific);
    amqp_buffer_set_write_index(buffer, frame_size);
}

static void amqp_sasl_mechanisms_feld_encoder(amqp_connection_t *connection, amqp_buffer_t *buffer, void *ignored)
{
    amqp_sasl_mechanisms_encode(connection->context, buffer);
}

void amqp_encode_sasl_mechanisms_frame(amqp_connection_t *connection, amqp_buffer_t *buffer)
{
    assert(connection && buffer);
    amqp_encode_frame(connection, buffer, amqp_sasl_mechanisms_list_descriptor, AMQP_SASL_FRAME_TYPE, DEFAULT_TYPE_SPECIFIC_FIELD, amqp_sasl_mechanisms_feld_encoder, DEFAULT_FIELDS_ENCODER_ARG);
}

static void amqp_sasl_init_feld_encoder(amqp_connection_t *connection, amqp_buffer_t *buffer, void *arg)
{
    amqp_sasl_plugin_t *sasl_plugin = (amqp_sasl_plugin_t *) arg;

    assert(sasl_plugin && sasl_plugin->mechanism_name);

    amqp_encode_symbol(connection->context, buffer, sasl_plugin->mechanism_name);
    amqp_sasl_plugin_initial_response(connection->context, sasl_plugin, buffer, &connection->identity_hooks);
    amqp_encode_string(connection->context, buffer, connection->socket.hostname);
}

void amqp_encode_sasl_init_frame(amqp_connection_t *connection, amqp_buffer_t *buffer, amqp_sasl_plugin_t *sasl_plugin)
{

    amqp_encode_frame(connection, buffer, amqp_sasl_init_list_descriptor, AMQP_SASL_FRAME_TYPE, DEFAULT_TYPE_SPECIFIC_FIELD, amqp_sasl_init_feld_encoder, sasl_plugin);
}
