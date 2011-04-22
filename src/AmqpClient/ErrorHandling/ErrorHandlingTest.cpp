#include "TestHarness.h"
#include "Context/ContextTestSupport.h"
#include "ErrorHandling/ErrorHandling.h"
#include "ErrorHandling/ErrorHandlingTestSupport.h"

namespace
{
    t::Context *c;

    void SetUp()
    {
        c = new t::Context();
    }

    void TearDown()
    {
        delete c;
    }
}

TEST(ErrorHandling, amqp_error_code_range)
{
    ASSERT(AMQP_ERROR_CODE_BASE > sys_nerr);
}

TEST(ErrorHandling, spike)
{
//    const char *s = strerror(1000);
//    SOUTS(s);
//    SOUTV(sys_nerr);
//
//    read(4, 0, 0);
//    amqp_io_error(c->context(), "ooops");
}

