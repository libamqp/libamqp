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

#include "Context/ContextTestSupport.h"
#include "Plugin/Sasl.h"
#include "AmqpTypes/AmqpSymbol.h"

#include "debug_helper.h"

SUITE(Context)
{
    class BaseSaslPluginsFixture : public ContextFixture
    {
    public:
        BaseSaslPluginsFixture()
        {
            cleanup_count = 0;
        }
        ~BaseSaslPluginsFixture()
        {
        }

        amqp_sasl_plugin_t *create_and_register_plugin(const char *name)
        {
            amqp_sasl_plugin_t *p = AMQP_MALLOC(context, amqp_sasl_plugin_t);
            p->mechanism_name = name;
            p->plugin_cleanup_handler = plugin_cleanup;
            amqp_context_register_sasl_plugin(context, p);
            return p;
        }

        static int cleanup_count;
        static void plugin_cleanup(amqp_context_t *context, amqp_sasl_plugin_t *plugin);

    public:
    };

    int BaseSaslPluginsFixture::cleanup_count = 0;

    void BaseSaslPluginsFixture::plugin_cleanup(amqp_context_t *context, amqp_sasl_plugin_t *plugin)
    {
        AMQP_FREE(context, plugin);
        cleanup_count++;
    }


    TEST_FIXTURE(BaseSaslPluginsFixture, sasl_plugins_cleanup_called)
    {
        create_and_register_plugin("ANONYMOUS");
        create_and_register_plugin("PLAIN");
        amqp_context_free_sasl_plugins(context);
        CHECK_EQUAL(2, cleanup_count);
    }

    TEST_FIXTURE(BaseSaslPluginsFixture, sasl_plugins_iterate_with_nothing_registered)
    {
        amqp_sasl_plugin_node_t *list;

        CHECK_EQUAL((void *) 0, amqp_context_first_sasl_plugin(context, &list));
    }

    class SaslPluginsFixture : public BaseSaslPluginsFixture
    {
    public:
        SaslPluginsFixture()
        {
            symbol_anonymous = amqp_symbol_create_from_cstr(context, "ANONYMOUS");
            symbol_plain = amqp_symbol_create_from_cstr(context, "PLAIN");
            symbol_pretty = amqp_symbol_create_from_cstr(context, "PRETTY");
        }
        ~SaslPluginsFixture()
        {
            amqp_context_free_sasl_plugins(context);

            amqp_symbol_cleanup(context, symbol_anonymous);
            amqp_symbol_cleanup(context, symbol_plain);
            amqp_symbol_cleanup(context, symbol_pretty);
        }

    public:
        amqp_symbol_t *symbol_anonymous;
        amqp_symbol_t *symbol_plain;
        amqp_symbol_t *symbol_pretty;
    };

    TEST_FIXTURE(SaslPluginsFixture, sasl_plugins_lookup)
    {
        amqp_sasl_plugin_t *p1 = create_and_register_plugin("ANONYMOUS");
        amqp_sasl_plugin_t *p2 = create_and_register_plugin("PLAIN");

        CHECK_EQUAL((void *) 0, amqp_context_lookup_sasl_plugin(context, symbol_pretty));
        CHECK_EQUAL(p2, amqp_context_lookup_sasl_plugin(context, symbol_plain));
        CHECK_EQUAL(p1, amqp_context_lookup_sasl_plugin(context, symbol_anonymous));
    }


    TEST_FIXTURE(SaslPluginsFixture, sasl_plugins_registered_plugin_count)
    {
        CHECK_EQUAL(0, amqp_context_sasl_plugin_count(context));
        create_and_register_plugin("ANONYMOUS");
        CHECK_EQUAL(1, amqp_context_sasl_plugin_count(context));
        create_and_register_plugin("PLAIN");
        CHECK_EQUAL(2, amqp_context_sasl_plugin_count(context));
    }


    TEST_FIXTURE(SaslPluginsFixture, sasl_plugins_lookup_with_nothing_registered)
    {
        CHECK_EQUAL((void *) 0, amqp_context_lookup_sasl_plugin(context, symbol_pretty));
        CHECK_EQUAL((void *) 0, amqp_context_lookup_sasl_plugin(context, symbol_plain));
        CHECK_EQUAL((void *) 0, amqp_context_lookup_sasl_plugin(context, symbol_anonymous));
    }

    TEST_FIXTURE(SaslPluginsFixture, sasl_plugins_anonymous_over_plain)
    {
        amqp_sasl_plugin_node_t *list;

        create_and_register_plugin("ANONYMOUS");
        create_and_register_plugin("PLAIN");

        amqp_sasl_plugin_t *p = amqp_context_first_sasl_plugin(context, &list);
        CHECK_EQUAL("ANONYMOUS", p->mechanism_name);
        p = amqp_context_next_sasl_plugin(context, &list);
        CHECK_EQUAL("PLAIN", p->mechanism_name);

        CHECK_EQUAL((void *) 0, amqp_context_next_sasl_plugin(context, &list));
    }

    TEST_FIXTURE(SaslPluginsFixture, sasl_plugins_plain_over_anonymous)
    {
        amqp_sasl_plugin_node_t *list;

        create_and_register_plugin("PLAIN");
        create_and_register_plugin("ANONYMOUS");

        amqp_sasl_plugin_t *p = amqp_context_first_sasl_plugin(context, &list);
        CHECK_EQUAL("PLAIN", p->mechanism_name);
        p = amqp_context_next_sasl_plugin(context, &list);
        CHECK_EQUAL("ANONYMOUS", p->mechanism_name);

        CHECK_EQUAL((void *) 0, amqp_context_next_sasl_plugin(context, &list));
    }
}

