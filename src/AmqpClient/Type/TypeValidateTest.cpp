#include "TestHarness.h"
#include "ErrorHandling/ErrorHandling.h"

#include "Context/ContextTestSupport.h"
#include "Type/TypeValidate.h"

namespace
{
    void SetUp()
    {
    }

    void TearDown()
    {
    }
}

TEST(TypeValidate, char)
{
    CHECK(amqp_validate_char(0x00000020));
    CHECK(amqp_validate_char(0x0000d7ff));
    CHECK(amqp_validate_char(0x0000e000));
    CHECK(amqp_validate_char(0x0010ffff));
    CHECK(!amqp_validate_char(0x000d800));
    CHECK(!amqp_validate_char(0x0000dfff));
    CHECK(!amqp_validate_char(0x0011ffff));
}

