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

#include <assert.h>
#include <string.h>

#include "Context/Context.h"

#include "AmqpTypes/AmqpDescriptor.h"
#include "debug_helper.h"

static
amqp_symbolic_descriptor_t descriptors[] =
{
    { "amqp:open:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_OPEN_LIST, amqp_amqp_descriptor }},
    { "amqp:begin:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_BEGIN_LIST, amqp_amqp_descriptor }},
    { "amqp:attach:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_ATTACH_LIST, amqp_amqp_descriptor }},
    { "amqp:flow:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_FLOW_LIST, amqp_amqp_descriptor }},
    { "amqp:transfer:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_TRANSFER_LIST, amqp_amqp_descriptor }},
    { "amqp:disposition:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_DISPOSITION_LIST, amqp_amqp_descriptor }},
    { "amqp:detach:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_DETACH_LIST, amqp_amqp_descriptor }},
    { "amqp:end:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_END_LIST, amqp_amqp_descriptor }},
    { "amqp:close:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_CLOSE_LIST, amqp_amqp_descriptor }},
    { "amqp:error:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_ERROR_LIST, amqp_amqp_descriptor }},
    { "amqp:sasl-mechanisms:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_SASL_MECHANISMS_LIST, amqp_sasl_descriptor }},
    { "amqp:sasl-init:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_SASL_INIT_LIST, amqp_sasl_descriptor }},
    { "amqp:sasl-challenge:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_SASL_CHALLENGE_LIST, amqp_sasl_descriptor }},
    { "amqp:sasl-response:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_SASL_RESPONSE_LIST, amqp_sasl_descriptor }},
    { "amqp:sasl-outcome:list", { AMQP_DESCRIPTOR_DOMAIN, AMQP_FRAME_ID_SASL_OUTCOME_LIST, amqp_sasl_descriptor }},
    {0}
};

static size_t map_size()
{
    size_t size = sizeof(descriptors) / sizeof(amqp_symbolic_descriptor_t);
    size = size / 3 * 4;
    return size < 16 ? 16 : size;
}

amqp_map_t *amqp_load_descriptors(amqp_context_t *context)
{
    int i = 0;
    amqp_map_t *result = amqp_symbol_map_create(context, map_size());
    while (descriptors[i].symbolic)
    {
        amqp_symbol_t *symbol = amqp_symbol_create(context, descriptors[i].symbolic, strlen(descriptors[i].symbolic));
        amqp_symbol_map_put(context, result, symbol, &descriptors[i].descriptor);
        i++;
    }
    return result;
}

void amqp_descriptors_cleanup(amqp_context_t *context, amqp_map_t *map)
{
    amqp_symbol_map_cleanup(context, map);
}

amqp_descriptor_t *amqp_descriptor_lookup(amqp_map_t *map, amqp_symbol_t *symbol)
{
   return (amqp_descriptor_t *) amqp_symbol_map_get(map, symbol);
}
