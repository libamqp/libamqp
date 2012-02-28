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

#ifndef LIBAMQP_TRANSPORT_CONNECTION_TEST_SUPPORT_DUMMY_BROKER_HOLDER_H
#define LIBAMQP_TRANSPORT_CONNECTION_TEST_SUPPORT_DUMMY_BROKER_HOLDER_H

#include "libamqp_common.h"
#include "Context/TestSupport/ContextHolder.h"
#include "Transport/DummyBroker/DummyBroker.h"
#include "Transport/Connection/TestSupport/ConnectionHolder.h"

namespace TestSupport
{
    class DummyBrokerHolder :
        public virtual ContextHolder
    {
    public:
        DummyBrokerHolder(amqp_connection_test_hook_t *test_hooks);
        ~DummyBrokerHolder();

    private:
        DummyBrokerHolder();
        amqp_dummy_broker_t *dummy_broker;
    };
}
#endif
