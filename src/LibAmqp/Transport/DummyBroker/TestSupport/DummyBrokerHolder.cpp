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

#include "Transport/DummyBroker/TestSupport/DummyBrokerHolder.h"

#include "debug_helper.h"


namespace TestSupport
{
    DummyBrokerHolder::DummyBrokerHolder(amqp_connection_test_hook_t *test_hooks)
    {
        dummy_broker = amqp_dummy_broker_initialize(context, TestParameters::port, test_hooks);
    }

    DummyBrokerHolder::~DummyBrokerHolder()
    {
        amqp_dummy_broker_destroy(context, dummy_broker);
    }
}
