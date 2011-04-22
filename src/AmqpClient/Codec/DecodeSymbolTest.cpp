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

TEST(DecodeSymbol, DecodeLargeSymbol)
{
    c->transfer_from(test_data::foo_bar_symbol_32);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    CHECK_EQUAL(0xb3, type->format_code);
    CHECK_EQUAL((size_t) 0x05, type->position.index);
    CHECK_EQUAL((size_t) 0x06, type->position.size);
    CHECK_EQUAL("FooBar", (result = amqp_convert_bytes_to_cstr(type)));
}

TEST(DecodeSymbol, DecodeSymbol)
{
    c->transfer_from(test_data::foo_symbol_8);
    type = amqp_decode(c->context());

    ASSERT_VALID(type);
    CHECK_EQUAL(0xa3, type->format_code);
    CHECK_EQUAL((size_t) 0x02, type->position.index);
    CHECK_EQUAL((size_t) 0x03, type->position.size);
    CHECK_EQUAL("Foo", (result = amqp_convert_bytes_to_cstr(type)));
}

TEST(DecodeSymbol, DecodeInvalidSymbol)
{
    c->transfer_from(test_data::bad_symbol);
    c->context()->debug.level = 0;
    type = amqp_decode(c->context());

    ASSERT_INVALID(type);
    ASSERT_EQUAL(AMQP_ERROR_INVALID_SYMBOL_CHARACTER, type->invalid_cause);
}
