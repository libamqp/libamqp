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
#include "Transport/DummyBroker/DummyBroker.h"
#include "Context/ContextTestSupport.h"
#include "Transport/Connection.h"

#include "debug_helper.h"

SUITE(Transport)
{
    class ConnectionFixture : public SuiteContext::ContextFixture
    {
    public:
        ConnectionFixture();
        ~ConnectionFixture();
    public:
        amqp_dummy_broker_t *m_broker;
    };

    ConnectionFixture::ConnectionFixture()
    {
        m_broker = amqp_dummy_broker_initialize(context, 5000, 0);
    }

    ConnectionFixture::~ConnectionFixture()
    {
        amqp_dummy_broker_destroy(context, m_broker);
    }

    TEST_FIXTURE(ConnectionFixture, really_important_test)
    {
        amqp_connection_t *connection = amqp_connection_initialize(context);
        connection->state.start(context, &connection->state);
        amqp_connection_destroy(context, connection);
    }
}
