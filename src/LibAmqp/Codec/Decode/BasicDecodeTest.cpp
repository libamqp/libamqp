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

#include "Codec/Decode/Decode.h"
#include "Codec/Type/Type.h"
#include "TestData/TestData.h"
#include "debug_helper.h"

#include "Buffer/BufferTestSupport.h"

SUITE(BasicDecode)
{
    class BasicDecodeFixture : public SuiteBuffer::BufferFixture
    {
    public:
        BasicDecodeFixture() { }
        ~BasicDecodeFixture() { }
    public:
    };

    TEST_FIXTURE(BasicDecodeFixture, DecodeSixtyFourBitValueIntoType)
    {
        test_data::double_8.transfer_to(buffer);
        amqp_type_t type;
        amqp_ntoh_64(&type.value.b8, buffer, 1);
        CHECK_CLOSE(123.456, type.value.b8._double, 0.00001);
    }
}
