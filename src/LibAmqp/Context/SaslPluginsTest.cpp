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
    class SaslPluginsFixture : public ContextFixture
    {
    public:
        SaslPluginsFixture() : s1(0), s2(0), s3(0)
        {
            cleanup_count = 0;
        }
        ~SaslPluginsFixture()
        {
            amqp_symbol_cleanup(context, s1);
            amqp_symbol_cleanup(context, s2);
            amqp_symbol_cleanup(context, s3);
        }
        amqp_sasl_plugin_t *create_and_register_plugin(const char *name)
        {
            amqp_sasl_plugin_t *p = AMQP_MALLOC(context, amqp_sasl_plugin_t);
            p->mechanism_name = name;
            p->plugin_cleanup_callback = plugin_cleanup;
            amqp_context_register_sasl_plugin(context, p);
            return p;
        }

        static int cleanup_count;
        static void plugin_cleanup(amqp_context_t *context, amqp_sasl_plugin_t *plugin);

    public:
        amqp_symbol_t *s1;
        amqp_symbol_t *s2;
        amqp_symbol_t *s3;
    };

    int SaslPluginsFixture::cleanup_count = 0;


    void SaslPluginsFixture::plugin_cleanup(amqp_context_t *context, amqp_sasl_plugin_t *plugin)
    {
        AMQP_FREE(context, plugin);
        cleanup_count++;
        SOUTV(cleanup_count);
    }

    TEST_FIXTURE(SaslPluginsFixture, sasl_plugins_lookup_and_cleanup)
    {
        amqp_sasl_plugin_t *p1 = create_and_register_plugin("ANONYMOUS");
        s1 = amqp_symbol_create_from_cstr(context, "ANONYMOUS");

        amqp_sasl_plugin_t *p2 = create_and_register_plugin("PLAIN");
        s2 = amqp_symbol_create_from_cstr(context, "PLAIN");

        s3 = amqp_symbol_create_from_cstr(context, "BRETTY");

        CHECK_EQUAL((void *) 0, amqp_context_lookup_sasl_plugin(context, s3));
        CHECK_EQUAL(p2, amqp_context_lookup_sasl_plugin(context, s2));
        CHECK_EQUAL(p1, amqp_context_lookup_sasl_plugin(context, s1));

        amqp_context_free_sasl_plugins(context);
        CHECK_EQUAL(2, cleanup_count);
    }

}

