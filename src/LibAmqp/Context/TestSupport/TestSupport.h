/*
   Copyright 2011-2012 StormMQ Limited

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#ifndef LIBAMQP_CONTEXT_TEST_SUPPORT_TEST_HELPERS_H
#define LIBAMQP_CONTEXT_TEST_SUPPORT_TEST_HELPERS_H

#include <string.h>

#include <TestHarness.h>
#include "Context/Context.h"

#define CHECK_FLAG_SET(flag)   CHECK_EQUAL(1U, flag)
#define CHECK_FLAG_CLEAR(flag)   CHECK_EQUAL(0U, flag)

namespace t
{
    void dump_type(amqp_context_t *context, amqp_type_t *type, amqp_buffer_t *buffer);

    int compare_buffers(const unsigned char *lhs, size_t lhs_size, const unsigned char *rhs, size_t rhs_size);
    int compare_buffers(amqp_context_t *context, const unsigned char *expect, size_t expect_size, amqp_buffer_t *buffer);
}

#define ASSERT_VALID(type)  \
    CHECK_NOT_NULL(type); \
    CHECK(amqp_type_is_valid(type))

#define ASSERT_INVALID(type)  \
    CHECK_NOT_NULL(type); \
    CHECK(amqp_type_is_invalid(type))

#define CHECK_ARRAY(type) \
    CHECK(amqp_type_is_container(type)); \
    CHECK(amqp_type_is_array(type)); \
    CHECK(!amqp_type_is_map(type)); \
    CHECK(!amqp_type_is_list(type))

#define CHECK_LIST(type) \
    CHECK(amqp_type_is_container(type)); \
    CHECK(!amqp_type_is_array(type)); \
    CHECK(!amqp_type_is_map(type)); \
    CHECK(amqp_type_is_list(type))

#define CHECK_MAP(type) \
    CHECK(amqp_type_is_container(type)); \
    CHECK(!amqp_type_is_array(type)); \
    CHECK(amqp_type_is_map(type)); \
    CHECK(!amqp_type_is_list(type))

#define CHECK_BUFFERS_MATCH(buffer, byte_array) \
     CHECK(t::compare_buffers(context, byte_array.bytes(), byte_array.size(), buffer))

#endif

