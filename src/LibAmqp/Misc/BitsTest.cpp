/*
   Copyright 2011 StormMQ Limited

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

#include <TestHarness.h>
#include "Misc/Bits.h"
#include "debug_helper.h"

SUITE(Misc)
{
    static int trailing_zeros_by_binary_search(uint64_t v)
    {
        if (v & 0x1)
        {
            // special case for odd v (assumed to happen half of the time)
            return 0;
        }
        int c = 1;
        if ((v & 0xffffffff) == 0)
        {
            v >>= 32;
            c += 32;
        }
        if ((v & 0xffff) == 0)
        {
            v >>= 16;
            c += 16;
        }
        if ((v & 0xff) == 0)
        {
            v >>= 8;
            c += 8;
        }
        if ((v & 0xf) == 0)
        {
            v >>= 4;
            c += 4;
        }
        if ((v & 0x3) == 0)
        {
            v >>= 2;
            c += 2;
        }
        c -= v & 0x1;
        return c;
    }

    TEST(uint64_trailing_zeros)
    {
        uint64_t f = -1;
        for (int i = 0; i < 64; i++)
        {
            uint64_t b = f & -f;
            CHECK_EQUAL(trailing_zeros_by_binary_search(b), amqp_trailing_zeros_64(b));
            f = f & ~b;
        }
    }

    TEST(uint32_trailing_zeros)
    {
        uint32_t f = -1;
        for (int i = 0; i < 32; i++)
        {
            uint32_t b = f & -f;
            CHECK_EQUAL(trailing_zeros_by_binary_search(b), amqp_trailing_zeros_32(b));
            f = f & ~b;
        }
    }

    TEST(next_power_of_2)
    {
        uint32_t mask = 0x80000000;
        while (mask > 8)
        {
            CHECK_EQUAL(mask, amqp_next_power_two_32(mask - 5));
            mask = mask >> 1;
        }
    }

    TEST(next_power_of_2_64)
    {
        uint64_t mask = 0x8000000000000000ULL;
        while (mask > 8)
        {
            CHECK_EQUAL(mask, amqp_next_power_two_64(mask - 5));
            mask = mask >> 1;
        }
    }

    TEST(next_power_of_2_overflow)
    {
        uint64_t mask = 0xf000000000000000ULL;
        CHECK_EQUAL(0U, amqp_next_power_two_64(mask));
    }
}