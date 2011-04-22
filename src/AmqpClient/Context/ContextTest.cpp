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

#include "TestHarness.h"
#include "Context/ContextTestSupport.h"
#include "Context/Context.h"

namespace
{
    t::Context *context;

    void SetUp()
    {
        context = 0;
    }

    void TearDown()
    {
        delete context;
    }
}

TEST(Context, setup)
{
    static unsigned char bytes[] = { 0x01, 0xa5, 0x03 };
    t::ByteArray buffer(bytes, sizeof(bytes));
    context = new t::Context(buffer);
    ASSERT_EQUAL(1, context->context()->decode.buffer->bytes[0]);
    ASSERT_EQUAL(0xa5, context->context()->decode.buffer->bytes[1]);
}

TEST(Context, local)
{
    static unsigned char bytes[] = { 0x01, 0xa5, 0x03 };
    t::ByteArray buffer(bytes, sizeof(bytes));
    t::Context local(buffer);
    ASSERT_EQUAL(1, local.context()->decode.buffer->bytes[0]);
    ASSERT_EQUAL(0xa5, local.context()->decode.buffer->bytes[1]);
}

