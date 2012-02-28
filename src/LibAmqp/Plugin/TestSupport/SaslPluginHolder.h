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

#ifndef LIBAMQP_PLUGIN_TEST_SUPPORT_SASL_PLUGIN_HOLDER_H
#define LIBAMQP_PLUGIN_TEST_SUPPORT_SASL_PLUGIN_HOLDER_H

#include "Context/TestSupport/ContextHolder.h"
#include "Plugin/Sasl.h"

namespace TestSupport
{
    class SaslPluginHolder : public virtual ContextHolder
    {
    public:
        SaslPluginHolder();
        ~SaslPluginHolder();

    public:
        amqp_sasl_plugin_t *sasl_plugin;
    };
}
#endif
