#include "TestHarness.h"
#include "Buffer/Buffer.h"
#include "Context/Context.h"
#include "ErrorHandling/ErrorHandling.h"

#include "Buffer/BufferTestSupport.h"

namespace
{
    unsigned char data[] = {
        0x70, 0x10, 0x07, 0x03, 0x01
    };

    amqp_buffer_t *buffer;
    amqp_context_t *context;

    void SetUp()
    {
        context = AMQP_NEW(amqp_context_t);
        buffer = AMQP_ALLOC(amqp_buffer_t, context);
    }

    void TearDown()
    {
        AMQP_DEALLOC(amqp_buffer_t, context, buffer);
        AMQP_DELETE(amqp_context_t, context);
    }
}

TEST(Buffer, amqp_buffer_wrap_array)
{
    amqp_buffer_puts(buffer, data, sizeof(data));

    ASSERT_EQUAL(buffer->limit.index, (size_t) 0);
    ASSERT_EQUAL(buffer->limit.size, sizeof(data));

    for (size_t i = 0; i < sizeof(data); i++)
    {
        ASSERT_EQUAL(buffer->bytes[i], data[i]);
    }
}

TEST(Buffer, deletion)
{
    AMQP_DEALLOC(amqp_buffer_t, context, buffer);
    ASSERT_EQUAL(0, buffer);
}

TEST(Buffer, amqp_buffer_at_end)
{
    amqp_buffer_puts(buffer, data, sizeof(data));
    amqp_buffer_reset(buffer);
    CHECK(amqp_buffer_at_end(buffer));
}

TEST(Buffer, getc)
{
    amqp_buffer_puts(buffer, data, sizeof(data));
    CHECK_EQUAL(amqp_buffer_getc(buffer), 0x70);
    CHECK_EQUAL(amqp_buffer_getc(buffer), 0x10);
    CHECK_EQUAL(amqp_buffer_getc(buffer), 0x07);
    CHECK_EQUAL(amqp_buffer_getc(buffer), 0x03);
    CHECK_EQUAL(amqp_buffer_getc(buffer), 0x01);
    CHECK(amqp_buffer_at_end(buffer));
    CHECK_EQUAL(amqp_buffer_getc(buffer), -1);
}

TEST(Buffer,  amqp_ntoh_16_unsigned)
{
    uint16_t v;
    unsigned char data[] = { 0x01, 0x02 };

    amqp_buffer_puts(buffer, data, sizeof(data));

    v = amqp_ntoh_16(buffer, 0)._ushort;
    ASSERT_EQUAL(0x0102, v);
}

TEST(Buffer, amqp_ntoh_16)
{
    int16_t v;
    unsigned char data[] = { 0xff, 0xfe };

    amqp_buffer_puts(buffer, data, sizeof(data));

    v = amqp_ntoh_16(buffer, 0)._short;
    ASSERT_EQUAL(-2, v);
}

TEST(Buffer, amqp_ntoh_32)
{
    uint32_t v;
    unsigned char data[] = { 0x01, 0x02, 0x04, 0x08 };

    amqp_buffer_puts(buffer, data, sizeof(data));

    v = amqp_ntoh_32(buffer, 0)._uint;
    ASSERT_EQUAL(0x01020408U, v);
}

TEST(Buffer, amqp_ntoh_64)
{
    uint64_t v;
    unsigned char data[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

    amqp_buffer_puts(buffer, data, sizeof(data));

    v = amqp_ntoh_64(buffer, 0)._ulong;
    ASSERT_EQUAL(0x0102040810204080UL, v);
}

TEST(Buffer,  amqp_hton_16)
{
    amqp_two_byte_t v;

    v._short = -2;
    amqp_hton_16(buffer, v);
    ASSERT_EQUAL((size_t) 0, buffer->limit.index);
    ASSERT_EQUAL((size_t) 2, buffer->limit.size);
    ASSERT_EQUAL(0xfe, buffer->bytes[1]);

    ASSERT_EQUAL(-2, amqp_ntoh_16(buffer, 0)._short);
}

TEST(Buffer,  amqp_hton_32)
{
    amqp_four_byte_t v;

    v._int = -2;
    amqp_hton_32(buffer, v);
    ASSERT_EQUAL((size_t) 0, buffer->limit.index);
    ASSERT_EQUAL((size_t) 4, buffer->limit.size);
    ASSERT_EQUAL(0xfe, buffer->bytes[3]);

    ASSERT_EQUAL(-2, amqp_ntoh_32(buffer, 0)._int);
}

TEST(Buffer,  amqp_hton_64)
{
    amqp_eight_byte_t v;

    v._long = -2;
    amqp_hton_64(buffer, v);
    ASSERT_EQUAL((size_t) 0, buffer->limit.index);
    ASSERT_EQUAL((size_t) 8, buffer->limit.size);
    ASSERT_EQUAL(0xfe, buffer->bytes[7]);

    ASSERT_EQUAL(-2, amqp_ntoh_64(buffer, 0)._long);
}

TEST(Buffer, amqp_buffer_read_size_one)
{
    unsigned char data[] = { 0x01, 0x02, 0x0a, 0x0e };
    amqp_buffer_puts(buffer, data, sizeof(data));

    ASSERT_EQUAL(1U, amqp_buffer_read_size(buffer, 1));
    ASSERT_EQUAL(2U, amqp_buffer_read_size(buffer, 1));
    ASSERT_EQUAL(10U, amqp_buffer_read_size(buffer, 1));
    ASSERT_EQUAL(14U, amqp_buffer_read_size(buffer, 1));
    ASSERT_EQUAL(-1U, amqp_buffer_read_size(buffer, 1));
}

TEST(Buffer, amqp_buffer_read_size_four)
{
    unsigned char data[] = { 0, 0, 0, 0x0e, 0, 0, 2, 1 };
    amqp_buffer_puts(buffer, data, sizeof(data));

    ASSERT_EQUAL(14U, amqp_buffer_read_size(buffer, 4));
    ASSERT_EQUAL(513U, amqp_buffer_read_size(buffer, 4));
    ASSERT_EQUAL(-1U, amqp_buffer_read_size(buffer, 4));
}

TEST(Buffer, amqp_buffer_read_size_bad)
{
    unsigned char data[] = { 0, 0,0x0e};
    amqp_buffer_puts(buffer, data, sizeof(data));

    ASSERT_EQUAL(-1U, amqp_buffer_read_size(buffer, 4));
}
