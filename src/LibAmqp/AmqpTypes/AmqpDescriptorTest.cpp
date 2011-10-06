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

#include "AmqpTypes/AmqpTypesTestSupport.h"
#include "AmqpTypes/AmqpDescriptor.h"

SUITE(AmqpTypes)
{
    class AmqpDescriptorFixture  : public AmqpTypesFixture
    {
    public:
        AmqpDescriptorFixture();
        ~AmqpDescriptorFixture();

        static const char *descriptor;

    public:
        amqp_symbol_t symbol;
        amqp_map_t *map;
    };

    const char *AmqpDescriptorFixture::descriptor = "amqp:sasl-mechanisms:list";

    AmqpDescriptorFixture::AmqpDescriptorFixture()
    {
        map = amqp_load_descriptors(context);
        amqp_symbol_initialize_reference(&symbol, 0, (const unsigned char *) descriptor, strlen(descriptor));
    }

    AmqpDescriptorFixture::~AmqpDescriptorFixture()
    {
        amqp_symbol_cleanup(context, &symbol);
        amqp_descriptors_cleanup(context, map);
    }

    TEST_FIXTURE(AmqpDescriptorFixture, lookup_descriptor_using_map_api)
    {
        amqp_descriptor_t *d = (amqp_descriptor_t *) amqp_symbol_map_get(map, &symbol);

        CHECK_EQUAL(0x00000000U, d->domain);
        CHECK_EQUAL(0x00000040U, d->id);
    }

    TEST_FIXTURE(AmqpDescriptorFixture, lookup_descriptor)
    {
        amqp_descriptor_t *d = amqp_descriptor_lookup(map, &symbol);

        CHECK_EQUAL(0x00000000U, d->domain);
        CHECK_EQUAL(0x00000040U, d->id);
    }

    TEST_FIXTURE(AmqpDescriptorFixture, lookup_descriptor_using_context)
    {
        amqp_descriptor_t *d = amqp_context_descriptor_lookup(context, &symbol);

        CHECK_EQUAL(0x00000000U, d->domain);
        CHECK_EQUAL(0x00000040U, d->id);
    }
}
