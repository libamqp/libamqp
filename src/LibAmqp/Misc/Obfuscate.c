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

#include <stdint.h>
#include "platform_limits.h"

#include "Context/Context.h"
#include "Misc/Obfuscate.h"

void amqp_obfuscate(uint32_t mask, unsigned char *dest, const unsigned char *source, size_t size)
{
    int bits = 32;
    int rotate = 0;
    int i;

    for (i = 0; i < size; i++)
    {
        dest[i] = source[i] ^ ((mask >> rotate) & 0x000000ff);
        rotate = (rotate + 1) % bits;
    }
}
