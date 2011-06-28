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

#ifndef LIBAMQP_CODEC_DECODE_DECODE_TEST_SUPPORT_H
#define LIBAMQP_CODEC_DECODE_DECODE_TEST_SUPPORT_H

#include <TestHarness.h>

#include "Codec/CodecTestSupport.h"

namespace SuiteCodecDecode
{
    class DecodeFixture : public SuiteCodec::CodecFixture
    {
    public:
        DecodeFixture() : result(0) {}
        ~DecodeFixture()
        {
            AMQP_FREE(result);
        }

    public:
        char *result;
    };
}

#endif
