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
#include "ErrorHandling/ErrorHandling.h"

#include "Codec/CodecTestSupport.h"
#include "Codec/Decode.h"
#include "Type/Type.h"

namespace
{
    t::Context *c;
    amqp_type_t *type;

    void SetUp()
    {
        c = new t::Context();
        type = 0;
    }

    void TearDown()
    {
        AMQP_DEALLOC(amqp_type_t, c->context(), type);
        delete c;
    }
}

TEST(DecodeNumber, Float)
{
    c->transfer_from(test_data::float_4);

    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(123.456f, type->value.b4._float);
}

TEST(DecodeNumber, Double)
{
    c->transfer_from(test_data::double_8);

    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(123.456, type->value.b8._double);
}

