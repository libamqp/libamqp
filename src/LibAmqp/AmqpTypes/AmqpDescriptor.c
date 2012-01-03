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

#include <assert.h>
#include <string.h>

#include "Context/Context.h"

#include "AmqpTypes/AmqpDescriptor.h"
#include "debug_helper.h"

static
amqp_symbolic_descriptor_t descriptors[] =
{
    { "amqp:open:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_open_list_descriptor, amqp_transport_descriptor_space}},
    { "amqp:begin:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_begin_list_descriptor, amqp_transport_descriptor_space}},
    { "amqp:attach:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_attach_list_descriptor, amqp_transport_descriptor_space}},
    { "amqp:flow:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_flow_list_descriptor, amqp_transport_descriptor_space}},
    { "amqp:transfer:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_transfer_list_descriptor, amqp_transport_descriptor_space}},
    { "amqp:disposition:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_disposition_list_descriptor, amqp_transport_descriptor_space}},
    { "amqp:detach:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_detach_list_descriptor, amqp_transport_descriptor_space}},
    { "amqp:end:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_end_list_descriptor, amqp_transport_descriptor_space}},
    { "amqp:close:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_close_list_descriptor, amqp_transport_descriptor_space}},

    { "amqp:error:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_error_list_descriptor, amqp_definition_descriptor_space}},

    { "amqp:received:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_received_list_descriptor, amqp_delivery_state_descriptor_space}},
    { "amqp:accepted:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_accepted_list_descriptor, amqp_delivery_state_descriptor_space}},
    { "amqp:rejected:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_rejected_list_descriptor, amqp_delivery_state_descriptor_space}},
    { "amqp:released:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_released_list_descriptor, amqp_delivery_state_descriptor_space}},
    { "amqp:modified:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_modified_list_descriptor, amqp_delivery_state_descriptor_space}},

    { "amqp:source:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_source_list_descriptor, amqp_addressing_descriptor_space}},
    { "amqp:target:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_target_list_descriptor, amqp_addressing_descriptor_space}},
    { "amqp:delete-on-close:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_delete_on_close_list_descriptor, amqp_addressing_descriptor_space}},
    { "amqp:delete-on-no-links:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_delete_on_no_links_list_descriptor, amqp_addressing_descriptor_space}},
    { "amqp:delete-on-no-messages:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_delete_on_no_messages_list_descriptor, amqp_addressing_descriptor_space}},
    { "amqp:delete-on-no-links-or-messages:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_delete_on_no_links_or_messages_list_descriptor, amqp_addressing_descriptor_space}},

    { "amqp:coordinator:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_coordinator_list_descriptor, amqp_transactions_descriptor_space}},
    { "amqp:declare:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_declare_list_descriptor, amqp_transactions_descriptor_space}},
    { "amqp:discharge:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_discharge_list_descriptor, amqp_transactions_descriptor_space}},
    { "amqp:declared:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_declared_list_descriptor, amqp_transactions_descriptor_space}},
    { "amqp:transactional-state:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_transactional_state_list_descriptor, amqp_transactions_descriptor_space}},

    { "amqp:sasl-mechanisms:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_sasl_mechanisms_list_descriptor, amqp_sasl_descriptor_space}},
    { "amqp:sasl-init:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_sasl_init_list_descriptor, amqp_sasl_descriptor_space}},
    { "amqp:sasl-challenge:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_sasl_challenge_list_descriptor, amqp_sasl_descriptor_space}},
    { "amqp:sasl-response:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_sasl_response_list_descriptor, amqp_sasl_descriptor_space}},
    { "amqp:sasl-outcome:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_sasl_outcome_list_descriptor, amqp_sasl_descriptor_space}},
    
    { "amqp:header:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_header_list_descriptor, amqp_message_format_descriptor_space}},
    { "amqp:delivery-annotations:map", {AMQP_DESCRIPTOR_DOMAIN, amqp_delivery_annotations_map_descriptor, amqp_message_format_descriptor_space}},
    { "amqp:message-annotations:map", {AMQP_DESCRIPTOR_DOMAIN, amqp_message_annotations_map_descriptor, amqp_message_format_descriptor_space}},
    { "amqp:properties:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_properties_list_descriptor, amqp_message_format_descriptor_space}},
    { "amqp:application-properties:map", {AMQP_DESCRIPTOR_DOMAIN, amqp_application_properties_map_descriptor, amqp_message_format_descriptor_space}},
    { "amqp:data:binary", {AMQP_DESCRIPTOR_DOMAIN, amqp_data_binary_descriptor, amqp_message_format_descriptor_space}},
    { "amqp:amqp-sequence:list", {AMQP_DESCRIPTOR_DOMAIN, amqp_amqp_sequence_list_descriptor, amqp_message_format_descriptor_space}},
    { "amqp:amqp-value:wildcard", {AMQP_DESCRIPTOR_DOMAIN, amqp_amqp_value_wildcard_descriptor, amqp_message_format_descriptor_space}},
    { "amqp:footer:map", {AMQP_DESCRIPTOR_DOMAIN, amqp_footer_map_descriptor, amqp_message_format_descriptor_space}},
    {0}
};

static size_t map_size()
{
    size_t size = sizeof(descriptors) / sizeof(amqp_symbolic_descriptor_t);
    size = size / 3 * 4;
    return size < 16 ? 16 : size;
}

amqp_hash_table_t *amqp_load_descriptors(amqp_context_t *context)
{
    int i = 0;
    amqp_hash_table_t *result = amqp_symbol_hash_table_create(context, map_size());
    while (descriptors[i].symbolic)
    {
        amqp_symbol_t *symbol = amqp_symbol_create(context, descriptors[i].symbolic, strlen(descriptors[i].symbolic));
        amqp_symbol_hash_table_put(context, result, symbol, &descriptors[i].descriptor);
        i++;
    }
    return result;
}

void amqp_descriptors_cleanup(amqp_context_t *context, amqp_hash_table_t *map)
{
    amqp_symbol_hash_table_cleanup(context, map);
}

amqp_descriptor_t *amqp_descriptor_lookup(amqp_hash_table_t *map, amqp_symbol_t *symbol)
{
   return (amqp_descriptor_t *) amqp_symbol_hash_table_get(map, symbol);
}

const char *amqp_descriptor_id_to_cstr(uint32_t id)
{
    int i = 0;
    while (descriptors[i].symbolic)
    {
        if (descriptors[i].descriptor.id == id)
        {
            return descriptors[i].symbolic;
        }
        i++;
    }
    return "unknown";
}
