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

TEST(DecodeString, UTF8String)
{
    c->transfer_from(test_data::hello_world);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    CHECK_EQUAL(0xa1, type->format_code);
    CHECK_EQUAL((size_t) 0x02, type->position.index);
    CHECK_EQUAL((size_t) 0x0b, type->position.size);
    CHECK_EQUAL("Hello World", (result = amqp_convert_bytes_to_cstr(type)));
}

TEST(DecodeString, LongUtf8String)
{
    c->transfer_from(test_data::hello_big_world);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    CHECK_EQUAL(0xb1, type->format_code);
    CHECK_EQUAL((size_t) 0x05, type->position.index);
    CHECK_EQUAL((size_t) 0x0f, type->position.size);
    CHECK_EQUAL("Hello Big World", (result = amqp_convert_bytes_to_cstr(type)));
}

