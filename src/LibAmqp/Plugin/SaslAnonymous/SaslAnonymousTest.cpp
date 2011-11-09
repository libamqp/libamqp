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
#include "Plugin/SaslAnonymous/SaslAnonymous.h"

#include "Context/ContextTestSupport.h"
#include "Context/Context.h"


SUITE(PluginSaslAnonymous)
{
    class PluginSaslAnonymousFixture : public SuiteContext::ContextFixture
    {
    public:
        PluginSaslAnonymousFixture();
        ~PluginSaslAnonymousFixture();

        amqp_sasl_plugin_t *plugin;
        amqp_sasl_plugin_t *instance;
        amqp_type_t *type;
    };

    PluginSaslAnonymousFixture::PluginSaslAnonymousFixture() :type(0)
    {
        plugin = amqp_plugin_sasl_anonymous_create(context);
        instance = amqp_sasl_plugin_instance_create(context, plugin);
    }
    PluginSaslAnonymousFixture::~PluginSaslAnonymousFixture()
    {
        amqp_deallocate_type(context, type);
        amqp_sasl_plugin_instance_cleanup(context, instance);
        amqp_sasl_plugin_cleanup(context, plugin);
    }

    static const char *provide_email(amqp_context_t *context) { return "joe@s.gl"; }
    static amqp_sasl_identity_t identity_hooks = {
        0,
        0,
        provide_email
    };

    TEST_FIXTURE(PluginSaslAnonymousFixture, fixture_balances_allocations)
    {
    }

    TEST_FIXTURE(PluginSaslAnonymousFixture, encode_initial_response)
    {
        type = amqp_sasl_plugin_initial_response(context, instance, decode_buffer, &identity_hooks);
        CHECK(amqp_type_is_binary(type));
        size_t expected_size = strlen(provide_email(context));

        CHECK_EQUAL(expected_size, type->position.size);

        CHECK_EQUAL('j', amqp_type_get_byte_at(type, 0));
        CHECK_EQUAL('o', amqp_type_get_byte_at(type, 1));
        CHECK_EQUAL('e', amqp_type_get_byte_at(type, 2));
        CHECK_EQUAL('@', amqp_type_get_byte_at(type, 3));
        CHECK_EQUAL('s', amqp_type_get_byte_at(type, 4));
        CHECK_EQUAL('.', amqp_type_get_byte_at(type, 5));
        CHECK_EQUAL('g', amqp_type_get_byte_at(type, 6));
        CHECK_EQUAL('l', amqp_type_get_byte_at(type, 7));
    }
}
