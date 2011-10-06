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

#include <stdlib.h>
#include <string.h>

#include "Buffer/Buffer.h"
#include "TestData/TestFrames.h"

namespace test_data
{
    test_data_def(minimal_frame_header,
        0, 0, 0, 8,
        2, 0, 0, 1
    );

    test_data_def(sasl_mechanisms_frame,
        0x00, 0x00, 0x00, 0x15,
        0x02, 0x01, 0x00, 0x00,
        0x00,
            0x53, 0x40,
            0xC0, 0x08, 0x01, 0xA3, 0x05, 0x50, 0x4C, 0x41, 0x49, 0x4E
    );

    test_data_def(sasl_mechanisms_frame_long,
        0x00, 0x00, 0x00, 0x1b,
        0x02, 0x01, 0x00, 0x00,
        0x00,
            0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
            0xC0, 0x08, 0x01, 0xA3, 0x05, 0x50, 0x4C, 0x41, 0x49, 0x4E
    );
    test_data_def(sasl_mechanisms_frame_symbol,
        0x00, 0x00, 0x00, 0x2e,
        0x02, 0x01, 0x00, 0x00,
        0x00,
            0xa3, 0x19, 'a', 'm', 'q', 'p', ':', 's', 'a', 's', 'l', '-', 'm', 'e', 'c', 'h', 'a', 'n', 'i', 's', 'm', 's', ':', 'l', 'i', 's', 't',
            0xC0, 0x08, 0x01, 0xA3, 0x05, 0x50, 0x4C, 0x41, 0x49, 0x4E
    );
}

