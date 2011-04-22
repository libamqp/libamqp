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
#include "Type/Type.h"

namespace
{
    amqp_type_t *type;
    t::Context *c;

    void SetUp()
    {
        c = new t::Context();
        c->context()->debug.level = 0;
        type = 0;
    }

    void TearDown()
    {
        AMQP_DEALLOC(amqp_type_t, c->context(), type);
        delete c;
    }
}

TEST(DecodeBadData, EndOfBuffer)
{
    c->transfer_from(test_data::empty_buffer);
    type = amqp_decode(c->context());

    ASSERT_NULL(type);
}

TEST(DecodeBadData, BadFormatCode)
{
    c->transfer_from(test_data::bad_format_code);
    type = amqp_decode_supress_messages(c->context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_UNKNOWN_FORMAT_CODE, type->invalid_cause);
}

TEST(DecodeBadData, BadFixedFormatCode)
{
    static unsigned char bad_data[] = {
        0x45,
    };

    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_UNKNOWN_FORMAT_CODE, type->invalid_cause);
}

TEST(DecodeBadData, InvalidFormatCode)
{
    static unsigned char bad_data[] = {
        0x3f,
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_UNKNOWN_FORMAT_CODE, type->invalid_cause);
}

TEST(DecodeBadData, DecodeBadULong)
{
    static unsigned char bad_data[] = {
        0x80, 0x10, 0x07, 0x03, 0x01
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, DecodeBadUInt)
{
    static unsigned char bad_data[] = {
        0x70, 0xff, 0xff
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, DecodeBadUShort)
{
    static unsigned char bad_data[] = {
        0x60, 0xff,
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, DecodeBadUByte)
{
    static unsigned char bad_data[] = {
        0x50
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, DecodeBadLong)
{
    static unsigned char bad_data[] = {
        0x81, 0x10, 0x07, 0x03, 0x01
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, DecodeBadInt)
{
    static unsigned char bad_data[] = {
        0x71, 0xff, 0xff
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, DecodeBadShort)
{
    static unsigned char bad_data[] = {
        0x61, 0xff,
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, DecodeBadByte)
{
    static unsigned char bad_data[] = {
        0x51
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, DecodeBadSymbol)
{
    static unsigned char bad_data[] = {
        0xa3, 3, 'F', 'o'
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, DecodeBadLargeSymbol)
{
    static unsigned char bad_data[] = {
        0xb3, 0, 0, 0, 6, 'F', 'o', 'o', 'B', 'a'
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, BadFloat)
{
    static unsigned char bad_data[] = {
        0x72, 0x42, 0xf6, 0xe9
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeBadData, BadDouble)
{
    static unsigned char bad_data[] = {
        0x82, 0x42, 0xf6, 0xe9 };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

TEST(DecodeString, DecodeBadString)
{
    static unsigned char bad_data[] = {
        0xA1, 0x0B, 'B', 'a', 'd', ' ', 'W', 'o', 'r', 'l', 'd'
    };
    t::ByteArray b(bad_data, sizeof(bad_data));
    t::Context c(b);
    type = amqp_decode_supress_messages(c.context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(1U, type->flags.is_invalid);
    ASSERT_EQUAL(AMQP_ERROR_BUFFER_WOULD_OVERRUN, type->invalid_cause);
}

