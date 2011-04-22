#include "TestHarness.h"
#include "ErrorHandling/ErrorHandling.h"

#include "Codec/CodecTestSupport.h"
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

TEST(DecodeZeroByteTypeTest, Null)
{
    c->transfer_from(test_data::null_0);
    type = amqp_decode(c->context());

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL(0x40, type->format_code);
    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x00, type->position.size);
    CHECK_EQUAL(1U, type->flags.is_null);
}

TEST(DecodeZeroByteTypeTest, True)
{
    c->transfer_from(test_data::true_0);
    type = amqp_decode(c->context());

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL(0x41, type->format_code);
    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x00, type->position.size);
    CHECK_EQUAL(0U, type->flags.is_null);

    CHECK(amqp_convert_bytes_to_boolean(type));
}

TEST(DecodeZeroByteTypeTest, False)
{
    c->transfer_from(test_data::false_0);
    type = amqp_decode(c->context());

    ASSERT_NOT_NULL(type);
    CHECK_EQUAL(0x42, type->format_code);
    CHECK_EQUAL((size_t) 0x01, type->position.index);
    CHECK_EQUAL((size_t) 0x00, type->position.size);
    CHECK_EQUAL(0U, type->flags.is_null);

    CHECK(!amqp_convert_bytes_to_boolean(type));
}

