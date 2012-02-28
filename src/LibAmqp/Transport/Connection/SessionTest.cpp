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
#include "Context/TestSupport/DecodeBufferHolder.h"
#include "Context/TestSupport/BufferHolder.h"
#include "Context/TestSupport/TypeHolder.h"
#include "Plugin/TestSupport/SaslPluginHolder.h"
#include "Transport/Connection/TestSupport/ConnectionHolder.h"
#include "Transport/Frame/TestSupport/FrameHolder.h"
#include "Transport/DummyBroker/TestSupport/DummyBrokerHolder.h"

#include "Transport/Connection/Session.h"

#include "debug_helper.h"

SUITE(Session)
{
    class UnconnectedConnectionFixture :
            public virtual TestSupport::ClientConnectionHolder,
            public virtual TestSupport::ContextHolder
    {
    public:
        UnconnectedConnectionFixture()
        {
            ConnectionHolder::running = true;
            connection->protocols = AMQP_PROTOCOL_SASL | AMQP_PROTOCOL_AMQP;
        }
        ~UnconnectedConnectionFixture()
        {
        }
    public:
    };

    TEST_FIXTURE(UnconnectedConnectionFixture, fixture_should_balance_allocations)
    {
    }

    TEST_FIXTURE(UnconnectedConnectionFixture, create_session)
    {
        amqp_session_t *session = amqp_session_create(connection);
        ASSERT(session == 0);
    }

    class SessionFixture :
            public TestSupport::DummyBrokerHolder,
            public virtual TestSupport::ClientConnectionHolder,
            public virtual TestSupport::ContextHolder,
            public virtual TestSupport::DecodeBufferHolder,
            public virtual TestSupport::SaslPluginHolder,
            public virtual TestSupport::TypeHolder,
            public virtual TestSupport::FrameHolder
    {
    public:
        SessionFixture() : TestSupport::DummyBrokerHolder(0), session(0)
        {
            ConnectionHolder::running = true;
            connection->protocols = AMQP_PROTOCOL_SASL | AMQP_PROTOCOL_AMQP;
            connection->state.connection.mode.client.connect(connection, "localhost", TestParameters::port);
            loop_until_connection_amqp_state_is("opened");
//            connection->trace_flags &= ~(AMQP_TRACE_CONNECTION_READER | AMQP_TRACE_CONNECTION_WRITER);
        }
        ~SessionFixture()
        {
            connection->trace_flags = 0;
            amqp_connection_shutdown(context, connection);
            loop_while_running();
            amqp_session_cleanup(session);
        }
    public:
        amqp_session_t *session;
    };

    TEST_FIXTURE(SessionFixture, fixture_should_balance_allocations)
    {
    }

    TEST_FIXTURE(SessionFixture, create_session)
    {
        session = amqp_session_create(connection);

    }

}
