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
#ifndef LIBAMQP_AMQP_TYPES_AMQP_DESCRIPTOR_H
#define LIBAMQP_AMQP_TYPES_AMQP_DESCRIPTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Context/Context.h"
#include "AmqpTypes/AmqpSymbol.h"

#define AMQP_DESCRIPTOR_DOMAIN  0x00

typedef enum amqp_frame_descriptor_id_t
{
    amqp_open_list_descriptor = 0x00000010,
    amqp_begin_list_descriptor = 0x00000011,
    amqp_attach_list_descriptor = 0x00000012,
    amqp_flow_list_descriptor = 0x00000013,
    amqp_transfer_list_descriptor = 0x00000014,
    amqp_disposition_list_descriptor = 0x00000015,
    amqp_detach_list_descriptor = 0x00000016,
    amqp_end_list_descriptor = 0x00000017,
    amqp_close_list_descriptor = 0x00000018,

    amqp_error_list_descriptor = 0x0000001d,

    amqp_received_list_descriptor = 0x00000023,
    amqp_accepted_list_descriptor = 0x00000024,
    amqp_rejected_list_descriptor = 0x00000025,
    amqp_released_list_descriptor = 0x00000026,
    amqp_modified_list_descriptor = 0x00000027,
    amqp_source_list_descriptor = 0x00000028,
    amqp_target_list_descriptor = 0x00000029,
    amqp_delete_on_close_list_descriptor = 0x0000002b,
    amqp_delete_on_no_links_list_descriptor = 0x0000002c,
    amqp_delete_on_no_messages_list_descriptor = 0x0000002d,
    amqp_delete_on_no_links_or_messages_list_descriptor = 0x0000002e,

    amqp_coordinator_list_descriptor = 0x00000030,
    amqp_declare_list_descriptor = 0x00000031,
    amqp_discharge_list_descriptor = 0x00000032,
    amqp_declared_list_descriptor = 0x00000033,
    amqp_transactional_state_list_descriptor = 0x00000034,

    amqp_sasl_mechanisms_list_descriptor = 0x00000040,
    amqp_sasl_init_list_descriptor = 0x00000041,
    amqp_sasl_challenge_list_descriptor = 0x00000042,
    amqp_sasl_response_list_descriptor = 0x00000043,
    amqp_sasl_outcome_list_descriptor = 0x00000044,

    amqp_header_list_descriptor = 0x00000070,
    amqp_delivery_annotations_map_descriptor = 0x00000071,
    amqp_message_annotations_map_descriptor = 0x00000072,
    amqp_properties_list_descriptor = 0x00000073,
    amqp_application_properties_map_descriptor = 0x00000074,
    amqp_data_binary_descriptor = 0x00000075,
    amqp_amqp_sequence_list_descriptor = 0x00000076,
    amqp_amqp_value_wildcard_descriptor = 0x00000077,
    amqp_footer_map_descriptor = 0x00000078,
} amqp_frame_descriptor_id_t;

typedef enum amqp_frame_type_type_t
{
    amqp_empty_frame = 1,
    amqp_unsupported_descriptor,
    amqp_transport_descriptor_space,
    amqp_sasl_descriptor_space,
    amqp_addressing_descriptor_space,
    amqp_definition_descriptor_space,
    amqp_delivery_state_descriptor_space,
    amqp_message_format_descriptor_space,
    amqp_transactions_descriptor_space,

} amqp_frame_type_type_t;

typedef struct amqp_descriptor_t
{
    uint32_t domain;
    uint32_t id;
    amqp_frame_type_type_t group;
} amqp_descriptor_t;

typedef struct amqp_symbolic_descriptor_t
{
    const char *symbolic;
    amqp_descriptor_t descriptor;
} amqp_symbolic_descriptor_t;

extern amqp_hash_table_t *amqp_load_descriptors(amqp_context_t *context);
extern void amqp_descriptors_cleanup(amqp_context_t *context, amqp_hash_table_t *map);
extern amqp_descriptor_t *amqp_descriptor_lookup(amqp_hash_table_t *map, amqp_symbol_t *symbol);
extern const char *amqp_descriptor_id_to_cstr(uint32_t id);

static inline
amqp_descriptor_t *amqp_context_descriptor_lookup(amqp_context_t *context, amqp_symbol_t *symbol)
{
    return amqp_descriptor_lookup(context->reference.amqp_descriptors, symbol);
}

#ifdef __cplusplus
}
#endif
#endif
