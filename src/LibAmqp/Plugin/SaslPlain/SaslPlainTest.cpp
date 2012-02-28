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
#include "Context/TestSupport/BufferHolder.h"

#include "Plugin/SaslPlain/SaslPlain.h"

SUITE(PluginSaslPlain)
{
    class PluginSaslPlainFixture :
            public virtual TestSupport::ContextHolder,
            public virtual TestSupport::BufferHolder
    {
    public:
        PluginSaslPlainFixture();
        ~PluginSaslPlainFixture();

        amqp_sasl_plugin_t *plugin;
        amqp_sasl_plugin_t *instance;
        amqp_type_t *type;
    };

    PluginSaslPlainFixture::PluginSaslPlainFixture() :type(0)
    {
        plugin = amqp_plugin_sasl_plain_create(context);
        instance = amqp_sasl_plugin_instance_create(context, plugin);
    }
    PluginSaslPlainFixture::~PluginSaslPlainFixture()
    {
        amqp_deallocate_type(context, type);
        amqp_sasl_plugin_instance_cleanup(context, instance);
        amqp_sasl_plugin_cleanup(context, plugin);
    }

    static const char *provide_login(amqp_context_t *context) { return "joe"; }
    static const char *provide_password(amqp_context_t *context) { return "secret"; }
    static amqp_sasl_identity_t identity_hooks = {
        provide_login,
        provide_password,
        0
    };

    TEST_FIXTURE(PluginSaslPlainFixture, fixture_balances_allocations)
    {
    }

    TEST_FIXTURE(PluginSaslPlainFixture, encode_initial_response)
    {
        type = amqp_sasl_plugin_initial_response_encode(context, instance, &identity_hooks, buffer);
        CHECK(amqp_type_is_binary(type));
        size_t expected_size = strlen(provide_login(context)) + strlen(provide_password(context)) + 2;
        CHECK_EQUAL(expected_size, type->position.size);
        CHECK_EQUAL(0, amqp_type_get_byte_at(type, 0));
        CHECK_EQUAL('j', amqp_type_get_byte_at(type, 1));
        CHECK_EQUAL('o', amqp_type_get_byte_at(type, 2));
        CHECK_EQUAL('e', amqp_type_get_byte_at(type, 3));
        CHECK_EQUAL(0, amqp_type_get_byte_at(type, 4));
        CHECK_EQUAL('s', amqp_type_get_byte_at(type, 5));
    }
}
