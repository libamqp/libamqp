#include "TestHarness.h"
#include "ErrorHandling/ErrorHandling.h"

#include "Codec/CodecTestSupport.h"

namespace
{
    t::Context *c;
    amqp_type_t *type;
    amqp_type_t *result;


    void SetUp()
    {
        c = new t::Context();
        type = 0;
        result = 0;
    }

    void TearDown()
    {
        AMQP_DEALLOC(amqp_type_t, c->context(), result);
        AMQP_DEALLOC(amqp_type_t, c->context(), type);
        delete c;
    }
}

TEST(EncodeFixedWidth, DecodeULong)
{
    type = amqp_encode_ulong(c->context(), 18446744073709551614UL);

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::ulong_8);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x08, type->position.size);
}

TEST(EncodeFixedWidth, DecodeLong)
{
    type = amqp_encode_long(c->context(), -2);

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::long_8);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x08, type->position.size);
}

TEST(EncodeFixedWidth, DecodeUInt)
{
    type = amqp_encode_uint(c->context(), 4294967294U);

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::uint_4);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x04, type->position.size);
}

TEST(EncodeFixedWidth, DecodeSmallULong)
{
    type = amqp_encode_small_ulong(c->context(), 254UL);

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x01, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::ulong_1);
}

TEST(EncodeFixedWidth, DecodeSmallUInt)
{
    type = amqp_encode_small_uint(c->context(), 254U);

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x01, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::uint_1);
}

TEST(EncodeFixedWidth, DecodeUShort)
{
    type = amqp_encode_ushort(c->context(), 65534);

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x02, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::ushort_2);
}

TEST(EncodeFixedWidth, DecodeShort)
{
    type = amqp_encode_short(c->context(), -2);

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x02, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::short_2);
}

TEST(EncodeFixedWidth, DecodeUByte)
{
    type = amqp_encode_ubyte(c->context(), 254);

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x01, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::ubyte_1);
}

TEST(EncodeFixedWidth, DecodeByte)
{
    type = amqp_encode_byte(c->context(), -2);

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x01, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::byte_1);
}

TEST(EncodeFixedWidth, DecodeInt)
{
    type = amqp_encode_int(c->context(), -2);

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::int_4);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x04, type->position.size);
}

TEST(EncodeFixedWidth, DecodeSmallLong)
{
    type = amqp_encode_small_long(c->context(), -4);

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::long_1);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x01, type->position.size);
}

TEST(EncodeFixedWidth, DecodeSmallInt)
{
    type = amqp_encode_small_int(c->context(), -3);

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::int_1);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x01, type->position.size);
}

TEST(EncodeFixedWidth, TimeStamp)
{
    type = amqp_encode_timestamp(c->context(), 1291654800000);

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::timestamp_8);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x08, type->position.size);
}

TEST(EncodeFixedWidth, TimeStamp2)
{
    type = amqp_encode_timestamp(c->context(), -1864105200000L);

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::timestamp_before_epoc_8);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x08, type->position.size);
}

TEST(EncodeFixedWidth, Float)
{
    type = amqp_encode_float(c->context(), 123.456f);

    ASSERT_NOT_NULL(type);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x04, type->position.size);

    amqp_buffer_put_buffer_contents(c->context()->decode.buffer, c->context()->encode.buffer);

    result = amqp_decode(c->context());
    ASSERT_NOT_NULL(result);
    ASSERT_VALID(result);
    ASSERT_EQUAL(123.456f, result->value.b4._float);
}

TEST(EncodeFixedWidth, Double)
{
    type = amqp_encode_double(c->context(), 123.456);

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::double_8);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x08, type->position.size);

    amqp_buffer_put_buffer_contents(c->context()->decode.buffer, c->context()->encode.buffer);

    result = amqp_decode(c->context());
    ASSERT_NOT_NULL(result);
    ASSERT_VALID(result);
    ASSERT_EQUAL(123.456, result->value.b8._double);
}

TEST(EncodeFixedWidth, uuid)
{
    amqp_uuid_t uuid = {0xf8, 0x1d, 0x4f, 0xae, 0x7d, 0xec, 0x11, 0xd0, 0xa7, 0x65, 0x00, 0xa0, 0xc9, 0x1e, 0x6b, 0xf6};
    type = amqp_encode_uuid(c->context(), &uuid);

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::uuid_16);

    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x10, type->position.size);
}

TEST(EncodeFixedWidth, uuid_t_width)
{
    ASSERT_EQUAL((size_t) 16, sizeof(amqp_uuid_t));
}

