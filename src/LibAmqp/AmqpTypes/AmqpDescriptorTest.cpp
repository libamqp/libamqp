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

#include "AmqpTypes/AmqpTypesTestFixture.h"
#include "AmqpTypes/AmqpDescriptor.h"

SUITE(AmqpTypes)
{
    class AmqpDescriptorTestFixture  : public AmqpTypesTestFixture
    {
    public:
        AmqpDescriptorTestFixture();
        ~AmqpDescriptorTestFixture();

        static const char *descriptor;

    public:
        amqp_symbol_t symbol;
        amqp_hash_table_t *map;
    };

    const char *AmqpDescriptorTestFixture::descriptor = "amqp:sasl-mechanisms:list";

    AmqpDescriptorTestFixture::AmqpDescriptorTestFixture()
    {
        map = amqp_load_descriptors(context);
        amqp_symbol_initialize(context, &symbol, descriptor, strlen(descriptor));
    }

    AmqpDescriptorTestFixture::~AmqpDescriptorTestFixture()
    {
        amqp_symbol_cleanup(context, &symbol);
        amqp_descriptors_cleanup(context, map);
    }

    TEST_FIXTURE(AmqpDescriptorTestFixture, lookup_descriptor_using_map_api)
    {
        amqp_descriptor_t *d = (amqp_descriptor_t *) amqp_symbol_hash_table_get(map, &symbol);

        CHECK(d != 0);
        if (d)
        {
            CHECK_EQUAL(0x00000000U, d->domain);
            CHECK_EQUAL(0x00000040U, d->id);
        }
    }

//extern int (*amqp_transport_decoder_t)(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list);
//extern int amqp_register_transport_decoder(context, const char *descriptor, uint32_t domain, uint32_t id, amqp_transport_decoder_t decoder);
//
//    TEST_FIXTURE(AmqpDescriptorTestFixture, lookup_descriptor_using_context)
//    {
//
//        amqp_descriptor_t *d = amqp_context_descriptor_lookup(context, &symbol);
//
//        CHECK(d != 0);
//        if (d)
//        {
//            CHECK_EQUAL(0x00000000U, d->domain);
//            CHECK_EQUAL(0x00000040U, d->id);
//        }
//    }
}
