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
    char *result;

    void SetUp()
    {
        c = new t::Context();
        type = 0;
        result = 0;
    }

    void TearDown()
    {
        AMQP_FREE(result);
        AMQP_DEALLOC(amqp_type_t, c->context(), type);
        delete c;
    }
}

TEST(DecodeInteger, DecodeVariedULong)
{
    c->transfer_from(test_data::varied_ulong_8);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(1154895135052768677UL, type->value.b8._ulong);
}

TEST(DecodeInteger, DecodeULong)
{
    c->transfer_from(test_data::ulong_8);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(18446744073709551614UL, type->value.b8._ulong);
}

TEST(DecodeInteger, DecodeUInt)
{
    c->transfer_from(test_data::uint_4);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(4294967294U, type->value.b4._uint);
}

TEST(DecodeInteger, DecodeSmallULong)
{
    c->transfer_from(test_data::ulong_1);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(254UL, type->value.b8._ulong);
}

TEST(DecodeInteger, DecodeSmallUInt)
{
    c->transfer_from(test_data::uint_1);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(254U, type->value.b4._uint);
}

TEST(DecodeInteger, DecodeUShort)
{
    c->transfer_from(test_data::ushort_2);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(65534, type->value.b2._ushort);
}

TEST(DecodeInteger, DecodeUByte)
{
    c->transfer_from(test_data::ubyte_1);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(254, type->value.b1._unsigned);
}

TEST(DecodeInteger, DecodeLong)
{
    c->transfer_from(test_data::long_8);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(-2, type->value.b8._long);
}

TEST(DecodeInteger, DecodeInt)
{
    c->transfer_from(test_data::int_4);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(-2, type->value.b4._int);
}

TEST(DecodeInteger, DecodeSmallLong)
{
    c->transfer_from(test_data::long_1);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(-4, type->value.b8._long);
}

TEST(DecodeInteger, DecodeSmallInt)
{
    c->transfer_from(test_data::int_1);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(-3, type->value.b4._int);
}

TEST(DecodeInteger, DecodeShort)
{
    c->transfer_from(test_data::short_2);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(-2, type->value.b2._short);
}

TEST(DecodeInteger, DecodeByte)
{
    c->transfer_from(test_data::byte_1);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(-2, type->value.b1._signed);
}

TEST(DecodeInteger, TimeStamp)
{
    c->transfer_from(test_data::timestamp_8);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(1291654800000L, type->value.b8._timestamp);
}

TEST(DecodeInteger, TimeStamp2)
{
    c->transfer_from(test_data::timestamp_before_epoc_8);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(-1864105200000L, type->value.b8._timestamp);
}

TEST(DecodeInteger, wchar)
{
    c->transfer_from(test_data::utf32_char);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    ASSERT_EQUAL(0x000020ac, type->value.b4._wchar);
}

