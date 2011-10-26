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
#include "Context/ErrorHandling.h"

#include "Codec/CodecTestSupport.h"
#include "Codec/Decode/Decode.h"
#include "Codec/Type/Type.h"

SUITE(IntegerDecode)
{
    class DecodeFixture : public SuiteCodec::CodecFixture
    {
    public:
        DecodeFixture() : result(0) {}
        ~DecodeFixture()
        {
            AMQP_FREE(context, result);
        }

    public:
        char *result;
    };
    
    TEST_FIXTURE(DecodeFixture, DecodeVariedULong)
    {
        load_decode_buffer(test_data::varied_ulong_8);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_ulong(type));
        CHECK_EQUAL(1154895135052768677ULL, type->value.b8._ulong);
    }

    TEST_FIXTURE(DecodeFixture, DecodeULong)
    {
        load_decode_buffer(test_data::ulong_8);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_ulong(type));
        CHECK_EQUAL(18446744073709551614ULL, type->value.b8._ulong);
    }

    TEST_FIXTURE(DecodeFixture, DecodeUInt)
    {
        load_decode_buffer(test_data::uint_4);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_uint(type));
        CHECK_EQUAL(4294967294U, type->value.b4._uint);
    }

    TEST_FIXTURE(DecodeFixture, DecodeSmallULong)
    {
        load_decode_buffer(test_data::ulong_small);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_ulong(type));
        CHECK_EQUAL(254UL, type->value.b8._ulong);
    }

    TEST_FIXTURE(DecodeFixture, DecodeSmallUInt)
    {
        load_decode_buffer(test_data::uint_small);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_uint(type));
        CHECK_EQUAL(254U, type->value.b4._uint);
    }
    
    TEST_FIXTURE(DecodeFixture, DecodeUintZero)
    {
        load_decode_buffer(test_data::uint_zero);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_uint(type));
        CHECK_EQUAL(0U, type->value.b4._uint);
    }

    TEST_FIXTURE(DecodeFixture, DecodeULongZero)
    {
        load_decode_buffer(test_data::ulong_zero);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_ulong(type));
        CHECK_EQUAL(0UL, type->value.b8._ulong);
    }

    TEST_FIXTURE(DecodeFixture, DecodeUShort)
    {
        load_decode_buffer(test_data::ushort_2);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_ushort(type));
        CHECK_EQUAL(65534, type->value.b2._ushort);
    }

    TEST_FIXTURE(DecodeFixture, DecodeUByte)
    {
        load_decode_buffer(test_data::ubyte_1);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_ubyte(type));
        CHECK_EQUAL(254, type->value.b1._unsigned);
    }

    TEST_FIXTURE(DecodeFixture, DecodeLong)
    {
        load_decode_buffer(test_data::long_8);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_long(type));
        CHECK_EQUAL(-2, type->value.b8._long);
    }

    TEST_FIXTURE(DecodeFixture, DecodeInt)
    {
        load_decode_buffer(test_data::int_4);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_int(type));
        CHECK_EQUAL(-2, type->value.b4._int);
    }

    TEST_FIXTURE(DecodeFixture, DecodeSmallLong)
    {
        load_decode_buffer(test_data::long_small);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_long(type));
        CHECK_EQUAL(-4, type->value.b8._long);
    }

    TEST_FIXTURE(DecodeFixture, DecodeSmallInt)
    {
        load_decode_buffer(test_data::int_1);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_int(type));
        CHECK_EQUAL(-3, type->value.b4._int);
    }

    TEST_FIXTURE(DecodeFixture, DecodeShort)
    {
        load_decode_buffer(test_data::short_2);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_short(type));
        CHECK_EQUAL(-2, type->value.b2._short);
    }

    TEST_FIXTURE(DecodeFixture, DecodeByte)
    {
        load_decode_buffer(test_data::byte_1);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_byte(type));
        CHECK_EQUAL(-2, type->value.b1._signed);
    }

    TEST_FIXTURE(DecodeFixture, TimeStamp)
    {
        load_decode_buffer(test_data::timestamp_8);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_timestamp(type));
        CHECK_EQUAL(1291654800000LL, type->value.b8._timestamp);
    }

    TEST_FIXTURE(DecodeFixture, TimeStamp2)
    {
        load_decode_buffer(test_data::timestamp_before_epoc_8);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_timestamp(type));
        CHECK_EQUAL(-1864105200000LL, type->value.b8._timestamp);
    }

    TEST_FIXTURE(DecodeFixture, wchar)
    {
        load_decode_buffer(test_data::utf32_char);
        type = amqp_decode(context, decode_buffer);

        ASSERT_VALID(type);
        CHECK(amqp_type_is_char(type));
        CHECK_EQUAL(0x000020ac, type->value.b4._wchar);
    }
}
