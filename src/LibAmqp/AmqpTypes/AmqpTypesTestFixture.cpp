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

#include <TestHarness.h>
#include "AmqpTypes/AmqpTypesTestFixture.h"

namespace SuiteAmqpTypes
{
    const char *AmqpTypesTestFixture::keys[40] = {
        "key_000", "key_001", "key_002", "key_003", "key_004", "key_005", "key_006", "key_007", "key_008", "key_009",
        "key_010", "key_011", "key_012", "key_013", "key_014", "key_015", "key_016", "key_017", "key_018", "key_019",
        "key_020", "key_021", "key_022", "key_023", "key_024", "key_025", "key_026", "key_027", "key_028", "key_029",
        "key_030", "key_031", "key_032", "key_033", "key_034", "key_035", "key_036", "key_037", "key_038",
        0
        };
    const char *AmqpTypesTestFixture::data[40] = {
        "Lorem ipsum ", "dolor sit amet ", "consectetur ", "adipisicing elit", ", sed do ",
        "eiusmod tempor ", "incididunt", "ut labore et ", "dolore magna ", "aliqua. Ut ",
        "enim ad minim ", "veniam, ", "quis ", "nostrud ", "exercitation ", "ullamco laboris ",
        "nisi ut aliquip ex ", "ea commodo ", "consequat. ", "Duis aute ", "irure dolor ", "in ",
        "reprehenderit ", "in", " voluptate ", "velit ", "esse ", "cillum ", "dolore eu ",
        "fugiat nulla ", "pariatur. ", "Excepteur sint ", "occaecat cupidatat ", "non proident, ",
        "sunt in ", "culpa qui ", "officia deserunt ", "mollit anim ", "id est laborum.",
        0
     };

    AmqpTypesTestFixture::AmqpTypesTestFixture()
    {
    }

    AmqpTypesTestFixture::~AmqpTypesTestFixture()
    {
    }

    int AmqpTypesTestFixture::compare(const char *lhs, const char *rhs)
    {
        return strcmp(lhs, rhs);
    }

    uint32_t AmqpTypesTestFixture::hash(const char *data)
    {
        return amqp_hash(data, strlen(data));
    }
    int AmqpTypesTestFixture::key_count()
    {
        int result = 0;
        while (keys[result]) result++;
        return result;
    }

}
