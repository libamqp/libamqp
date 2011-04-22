#include "TestHarness.h"
#include "ErrorHandling/ErrorHandling.h"

#include "Context/ContextTestSupport.h"
#include "Type/TypePool.h"

namespace
{
    amqp_type_t *type;
    t::Context *c;

    void SetUp()
    {
        c  = new t::Context();
        type = 0;
    }

    void TearDown()
    {
        delete c;
    }
}

TEST(Type, aray_realloc)
{
    int i;
    amqp_type_t **array_1 = amqp_allocate_amqp_type_t_array(15);
    amqp_type_t **array_2 = amqp_allocate_amqp_type_t_array(15);

    for (i = 0; i < 15; i++)
    {
        array_1[i] = amqp_alloc_amqp_type_t(c->context());
        array_2[i] = array_1[i];
    }

    array_1 = amqp_realloc_amqp_type_t_array(array_1, 16);
    for (i = 0; i < 15; i++)
    {
        ASSERT_EQUAL(array_2[i], array_1[i]);
    }
    CHECK_EQUAL(0, array_1[16]);

    amqp_deallocate_amqp_type_t_array(c->context(), array_1, 15);
    amqp_deallocate_amqp_type_t_array(c->context(), array_2, 0);
}

TEST(Type, flags)
{
    amqp_type_t type = { 0 };
    ASSERT_EQUAL(sizeof(unsigned) * 2, sizeof(type.flags));

    ASSERT(!amqp_type_is_map(&type));
    ASSERT(!amqp_type_is_list(&type));
    ASSERT(!amqp_type_is_array(&type));
    ASSERT(!amqp_type_is_compound(&type));
}

TEST(Type, flags_is_map)
{
    amqp_type_t type = { 0 };
    type.flags.structure.flags.is_map = true;

    ASSERT(amqp_type_is_compound(&type));
    ASSERT(amqp_type_is_map(&type));
    ASSERT(!amqp_type_is_list(&type));
    ASSERT(!amqp_type_is_array(&type));
}

TEST(Type, flags_is_list)
{
    amqp_type_t type = { 0 };
    type.flags.structure.flags.is_list = true;

    ASSERT(amqp_type_is_compound(&type));
    ASSERT(!amqp_type_is_map(&type));
    ASSERT(amqp_type_is_list(&type));
    ASSERT(!amqp_type_is_array(&type));
}

TEST(Type, flags_is_array)
{
    amqp_type_t type = { 0 };
    type.flags.structure.flags.is_array = true;

    ASSERT(amqp_type_is_compound(&type));
    ASSERT(!amqp_type_is_map(&type));
    ASSERT(!amqp_type_is_list(&type));
    ASSERT(amqp_type_is_array(&type));
}

static void copy_flags(amqp_type_t *t1, amqp_type_t *t2)
{
    t2->flags.structure = t1->flags.structure;
}

TEST(Type, flags_assign)
{
    amqp_type_t t1 = { 0 };
    amqp_type_t t2 = { 0 };

    t1.flags.structure.flags.is_list = true;
    copy_flags(&t1, &t2);

    ASSERT(amqp_type_is_compound(&t2));
    ASSERT(!amqp_type_is_map(&t2));
    ASSERT(amqp_type_is_list(&t2));
    ASSERT(!amqp_type_is_array(&t2));
}