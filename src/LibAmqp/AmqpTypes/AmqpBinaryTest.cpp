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

#include <TestHarness.h>
#include "AmqpTypes/AmqpTypesTestSupport.h"

#include "AmqpTypes/AmqpBinary.h"
#include "Codec/Type/TypeExtension.h"

#include "debug_helper.h"

SUITE(AmqpTypes)
{
    class AmqpBinaryFixture  : public AmqpTypesFixture
    {
    public:
        AmqpBinaryFixture();
        ~AmqpBinaryFixture();

    public:
        amqp_binary_t ref;
        amqp_binary_t *binary;
    };

    AmqpBinaryFixture::AmqpBinaryFixture() : binary(0)
    {
        memset(&ref, '\0', sizeof(amqp_binary_t));
    }

    AmqpBinaryFixture::~AmqpBinaryFixture()
    {
        amqp_binary_cleanup(context, &ref);
        amqp_binary_cleanup(context, binary);
    }

    TEST_FIXTURE(AmqpBinaryFixture, binary_type_create_from_type)
    {
        test_data::bin_8.transfer_to(buffer);
        type = amqp_decode(context, buffer);
        CHECK(amqp_type_is_binary(type));
        binary = amqp_binary_create_from_type(context, type);
        CHECK_EQUAL(12U, amqp_binary_size(binary));
    }

    TEST_FIXTURE(AmqpBinaryFixture, binary_type_copy_to)
    {
        test_data::bin_8.transfer_to(buffer);
        type = amqp_decode(context, buffer);
        binary = amqp_binary_create_from_type(context, type);

        uint8_t data[32];
        memset(data, '\0', sizeof(data));
        amqp_binary_to_bytes(binary, data, sizeof(data));
        CHECK_EQUAL((const char *) data, "binary array");
    }

    TEST_FIXTURE(AmqpBinaryFixture, binary_type_access)
    {
        test_data::bin_8.transfer_to(buffer);
        type = amqp_decode(context, buffer);
        binary = amqp_binary_create_from_type(context, type);

        CHECK_EQUAL('b', amqp_binary_byte_get_at(binary, 0));
        CHECK_EQUAL('i', amqp_binary_byte_get_at(binary, 1));
        CHECK_EQUAL('y', amqp_binary_byte_get_at(binary, 11));
    }
}
