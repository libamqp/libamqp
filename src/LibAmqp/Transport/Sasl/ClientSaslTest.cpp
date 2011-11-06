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
#include "Transport/Connection/Connection.h"
#include "Transport/Sasl/SaslTestSupport.h"
#include "Codec/CodecTestSupport.h"
#include "AmqpTypes/AmqpSymbol.h"
#include "AmqpTypes/AmqpMultiple.h"
#include "Plugin/SaslPlain/SaslPlain.h"
#include "Plugin/SaslAnonymous/SaslAnonymous.h"

#include "debug_helper.h"

SUITE(Sasl)
{
    class ClientSaslFixture : public SuiteSasl::BaseSaslFixture
    {
    public:
        ClientSaslFixture();
        ~ClientSaslFixture();

    public:
        amqp_symbol_t *symbol;
        amqp_multiple_symbol_t *multiple;
//        amqp_connection_t *connection;
    };

    ClientSaslFixture::ClientSaslFixture() : symbol(0), multiple(0)
    {
//        connection = amqp_connection_create(context);
        amqp_context_register_sasl_plugin(context, amqp_plugin_sasl_plain_create(context));
    }

    ClientSaslFixture::~ClientSaslFixture()
    {
        amqp_symbol_cleanup(context, symbol);
        amqp_multiple_symbol_cleanup(context, multiple);
//        amqp_connection_destroy(context, connection);
    }

//amqp_plugin_sasl_plain_create(context)
    TEST_FIXTURE(ClientSaslFixture, encode_sasl_init_frame)
    {
        CHECK(0);
    }
}
