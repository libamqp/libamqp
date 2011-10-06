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
    { "amqp:open:list", { 0x00000000, 0x00000010 }},
    { "amqp:begin:list", { 0x00000000, 0x00000011 }},
    { "amqp:attach:list", { 0x00000000, 0x00000012 }},
    { "amqp:flow:list", { 0x00000000, 0x00000013 }},
    { "amqp:transfer:list", { 0x00000000, 0x00000014 }},
    { "amqp:disposition:list", { 0x00000000, 0x00000015 }},
    { "amqp:detach:list", { 0x00000000, 0x00000016 }},
    { "amqp:end:list", { 0x00000000, 0x00000017 }},
    { "amqp:close:list", { 0x00000000, 0x00000018 }},
    { "amqp:error:list", { 0x00000000, 0x0000001d }},
    { "amqp:sasl-mechanisms:list", { 0x00000000, 0x00000040 }},
    { "amqp:sasl-init:list", { 0x00000000, 0x00000041 }},
    { "amqp:sasl-challenge:list", { 0x00000000, 0x00000042 }},
    { "amqp:sasl-response:list", { 0x00000000, 0x00000043 }},
    { "amqp:sasl-outcome:list", { 0x00000000, 0x00000044 }},
    {0}
};

static size_t map_size()
{
    size_t size = sizeof(descriptors) / sizeof(amqp_symbolic_descriptor_t);
    size = size / 3 * 4;
    return size < 16 ? 16 : size;
}

void amqp_load_descriptors(amqp_context_t *context, amqp_map_t *map)
{
    int i = 0;

    amqp_symbol_map_initialize(context, map, map_size());
    while (descriptors[i].symbolic)
    {
        amqp_symbol_t *symbol = amqp_symbol_create(context, 0, descriptors[i].symbolic, strlen(descriptors[i].symbolic));
        amqp_symbol_map_put(context, map, symbol, &descriptors[i].descriptor);
        i++;
    }
}

void amqp_descriptors_cleannup(amqp_context_t *context, amqp_map_t *map)
{
    amqp_symbol_map_cleanup(context, map);
}

amqp_descriptor_t *amqp_descriptor_lookup(amqp_map_t *map, amqp_symbol_t *symbol)
{
   return (amqp_descriptor_t *) amqp_symbol_map_get(map, symbol);
}
