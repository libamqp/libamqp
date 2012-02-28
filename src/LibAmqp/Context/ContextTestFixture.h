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

#ifndef LIBAMQP_CONTEXT_TEST_SUPPORT_H
#define LIBAMQP_CONTEXT_TEST_SUPPORT_H

#include "Context/TestSupport/ContextHolder.h"
#include "Context/TestSupport/DecodeBufferHolder.h"

namespace SuiteContext
{
    class ContextTestFixture :
            public virtual TestSupport::ContextHolder,
            public virtual TestSupport::DecodeBufferHolder
    {
    public:
        ContextTestFixture() {}
        ~ContextTestFixture() {}
    };
}

#endif

