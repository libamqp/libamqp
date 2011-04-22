#include "TestHarness.h"
#include "ErrorHandling/ErrorHandling.h"

#include "Codec/CodecTestSupport.h"
#include "Codec/EncodeFramingTypes.h"

#include <stdio.h>

namespace
{
    t::Context *c;
    amqp_type_t *type;
    amqp_context_t *context;

    void SetUp()
    {
        c = new t::Context();
        type = 0;
        context = c->context();
    }
    void TearDown()
    {
        AMQP_DEALLOC(amqp_type_t, context, type);
        delete c;
    }
}

TEST(EncodeComposite, multiple_true_symbol_null)
{
    type = amqp_encode_multiple_true(context, amqp_type_d(symbol));
    ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::multiple_true_symbol_null);
}

TEST(EncodeComposite, multiple_true_symbol_one_value)
{
    type =  amqp_encode_multiple_true(context, amqp_type_d(symbol), "Foo");
    ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::multiple_true_symbol_one_value);
}

TEST(EncodeComposite, multiple_true_symbol_many_values)
{
    type =  amqp_encode_multiple_true(context, amqp_type_d(symbol), "Foo", "Fum");
    ASSERT_BUFFERS_MATCH(context->encode.buffer, test_data::multiple_true_symbol_many_values);
}