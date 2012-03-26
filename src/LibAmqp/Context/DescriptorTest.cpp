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

#include <TestHarness.h>

#include "Context/TestSupport/ContextHolder.h"
#include "AmqpTypes/AmqpSymbol.h"

SUITE(Context)
{
    class DescriptorTestFixture  : public TestSupport::ContextHolder
    {
    public:
        DescriptorTestFixture();
        ~DescriptorTestFixture();

    public:
        amqp_symbol_t symbol;

    private:
        static const char *descriptor;
    };

    const char *DescriptorTestFixture::descriptor = "amqp:sasl-mechanisms:list";

    DescriptorTestFixture::DescriptorTestFixture()
    {
        amqp_symbol_initialize(context, &symbol, descriptor, strlen(descriptor));
    }

    DescriptorTestFixture::~DescriptorTestFixture()
    {
        amqp_symbol_cleanup(context, &symbol);
    }


    TEST_FIXTURE(DescriptorTestFixture, lookup_descriptor_using_symbol)
    {
        amqp_descriptor_t *d = amqp_context_descriptor_lookup(context, &symbol);

        CHECK(d != 0);
        if (d)
        {
            CHECK_EQUAL(0x00000000U, d->domain);
            CHECK_EQUAL(0x00000040U, d->id);
        }
    }

    TEST_FIXTURE(DescriptorTestFixture, lookup_descriptor_using_domain_and_id)
    {
//        CHECK(0); // TODO
    }

}
