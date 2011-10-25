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
#include "Misc/Obfuscate.h"
#include "debug_helper.h"

SUITE(Misc)
{
    class ObfuscationFixture
    {
    public:
        ObfuscationFixture();
        ~ObfuscationFixture();
        void check_unobfuscated(size_t n);

    public:
        char buffer[65];
        char obfuscated[64];
        char unobfuscated[65];
        uint32_t mask;
    };

    ObfuscationFixture::ObfuscationFixture()
    {
        for (int i = 0; i < 64; i++)
        {
            buffer[i] = 'A' + (i % ('Z' - 'A' + 1));
        }
        buffer[64] = 0;
        mask = 0xe64a1dbd;
    }

    ObfuscationFixture::~ObfuscationFixture()
    {
    }

    void ObfuscationFixture::check_unobfuscated(size_t n)
    {
        amqp_obfuscate(mask, (unsigned char *) obfuscated, (unsigned char *) buffer, n);
        amqp_obfuscate(mask, (unsigned char *) unobfuscated, (unsigned char *) obfuscated, n);
        buffer[n] = unobfuscated[n] = 0;
        CHECK_EQUAL(buffer, unobfuscated);
    }

    TEST_FIXTURE(ObfuscationFixture, verify_obfuscate_unobfuscate_works)
    {
        check_unobfuscated(64);
    }

    TEST_FIXTURE(ObfuscationFixture, verify_obfuscate_unobfuscate_works_31)
    {
        check_unobfuscated(31);
    }

    TEST_FIXTURE(ObfuscationFixture, verify_obfuscate_unobfuscate_works_32)
    {
        check_unobfuscated(32);
    }
    
    TEST_FIXTURE(ObfuscationFixture, verify_obfuscate_unobfuscate_works_33)
    {
        check_unobfuscated(33);
    }
}