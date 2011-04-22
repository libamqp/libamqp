#include "TestHarness.h"
#include "ErrorHandling/ErrorHandling.h"

#include "Codec/CodecTestSupport.h"
#include "Codec/Encode.h"

#include "Type/Type.h"

namespace
{
    const unsigned char *binary = (unsigned char *) "binary array";
    const unsigned char *big_binary = (unsigned char *) "big binary array";

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

TEST(EncodeVariableWidth, amqp_encode_string_str_utf8)
{
    type = amqp_encode_string_utf8(c->context(), "Hello World");

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::hello_world);

    CHECK_EQUAL((size_t) 0x02, type->position.index);
    CHECK_EQUAL((size_t) 0x0b, type->position.size);
}

TEST(EncodeVariableWidth, amqp_encode_string_utf8)
{
    type = amqp_encode_string_utf8(c->context(), "Hello World");

    ASSERT_NOT_NULL(type);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::hello_world);

    CHECK_EQUAL((size_t) 0x02, type->position.index);
    CHECK_EQUAL((size_t) 0x0b, type->position.size);
}

TEST(EncodeVariableWidth, amqp_encode_string_str32_utf8)
{
    ASSERT_EQUAL((size_t) 257, strlen(test_data::lorem_ipsum));

    type = amqp_encode_string_utf8(c->context(), test_data::lorem_ipsum);

    ASSERT_NOT_NULL(type);

//    amqp_buffer_dump_as_byte_array(c->context()->encode.buffer, c->context()->config.putc);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::encoded_lorem_ipsum);

    CHECK_EQUAL((size_t) 0x05, type->position.index);
    CHECK_EQUAL((size_t) 257, type->position.size);
}

TEST(EncodeVariableWidth, amqp_encode_binary)
{
    type = amqp_encode_binary(c->context(), binary, strlen((const char *) binary));

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x02, type->position.index);
    CHECK_EQUAL((size_t) 12, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::bin_8);
}

TEST(EncodeVariableWidth, amqp_encode_binary_bin8)
{
    type = amqp_encode_binary_vbin8(c->context(), binary, strlen((const char *) binary));

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x02, type->position.index);
    CHECK_EQUAL((size_t) 12, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::bin_8);
}

TEST(EncodeVariableWidth, amqp_encode_binary_bin32)
{
    type = amqp_encode_binary_vbin32(c->context(), big_binary, strlen((const char *) big_binary));

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x05, type->position.index);
    CHECK_EQUAL((size_t) 16, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::bin_32);
}


TEST(EncodeVariableWidth, amqp_encode_symbol)
{
    type = amqp_encode_symbol(c->context(), "Foo");

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x02, type->position.index);
    CHECK_EQUAL((size_t) 3, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::foo_symbol_8);
}

TEST(EncodeVariableWidth, amqp_encode_symbol_sym8)
{
    type = amqp_encode_symbol_sym8(c->context(), "Foo");

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x02, type->position.index);
    CHECK_EQUAL((size_t) 3, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::foo_symbol_8);
}

TEST(EncodeVariableWidth, amqp_encode_symbol_sym32)
{
    type = amqp_encode_symbol_sym32(c->context(), "FooBar");

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x05, type->position.index);
    CHECK_EQUAL((size_t) 6, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::foo_bar_symbol_32);
}


TEST(EncodeVariableWidth, amqp_encode_symboln)
{
    type = amqp_encode_symboln(c->context(), "Foo", 3);

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x02, type->position.index);
    CHECK_EQUAL((size_t) 3, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::foo_symbol_8);
}

TEST(EncodeVariableWidth, amqp_encode_symbol_sym8n)
{
    type = amqp_encode_symbol_sym8n(c->context(), "Foo", 3);

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x02, type->position.index);
    CHECK_EQUAL((size_t) 3, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::foo_symbol_8);
}

TEST(EncodeVariableWidth, amqp_encode_symbol_sym32n)
{
    type = amqp_encode_symbol_sym32n(c->context(), "FooBar", 6);

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL((size_t) 0x05, type->position.index);
    CHECK_EQUAL((size_t) 6, type->position.size);
    ASSERT_BUFFERS_MATCH(c->context()->encode.buffer, test_data::foo_bar_symbol_32);
}

