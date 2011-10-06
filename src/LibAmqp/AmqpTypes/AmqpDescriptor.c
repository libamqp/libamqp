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

#include "AmqpTypes/AmqpDescriptor.h"
#include "debug_helper.h"

static
amqp_symbolic_descriptor_t descriptors[] =
{
    { "amqp:open:list", { 0x00000000, 0x00000010, amqp_amqp_descriptor }},
    { "amqp:begin:list", { 0x00000000, 0x00000011, amqp_amqp_descriptor }},
    { "amqp:attach:list", { 0x00000000, 0x00000012, amqp_amqp_descriptor }},
    { "amqp:flow:list", { 0x00000000, 0x00000013, amqp_amqp_descriptor }},
    { "amqp:transfer:list", { 0x00000000, 0x00000014, amqp_amqp_descriptor }},
    { "amqp:disposition:list", { 0x00000000, 0x00000015, amqp_amqp_descriptor }},
    { "amqp:detach:list", { 0x00000000, 0x00000016, amqp_amqp_descriptor }},
    { "amqp:end:list", { 0x00000000, 0x00000017, amqp_amqp_descriptor }},
    { "amqp:close:list", { 0x00000000, 0x00000018, amqp_amqp_descriptor }},
    { "amqp:error:list", { 0x00000000, 0x0000001d, amqp_amqp_descriptor }},
    { "amqp:sasl-mechanisms:list", { 0x00000000, 0x00000040, amqp_sasl_descriptor }},
    { "amqp:sasl-init:list", { 0x00000000, 0x00000041, amqp_sasl_descriptor }},
    { "amqp:sasl-challenge:list", { 0x00000000, 0x00000042, amqp_sasl_descriptor }},
    { "amqp:sasl-response:list", { 0x00000000, 0x00000043, amqp_sasl_descriptor }},
    { "amqp:sasl-outcome:list", { 0x00000000, 0x00000044, amqp_sasl_descriptor }},
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
        amqp_symbol_t *symbol = amqp_symbol_create(context, 0, (const unsigned char *) descriptors[i].symbolic, strlen(descriptors[i].symbolic));
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
