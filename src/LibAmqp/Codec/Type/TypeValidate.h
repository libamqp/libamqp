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

#ifndef LIBAMQP_TYPE_TYPE_VALIDATE_H
#define LIBAMQP_TYPE_TYPE_VALIDATE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "libamqp_common.h"
#include "Context/Context.h"
#include "Codec/Type/Type.h"

inline static int amqp_validate_char(wchar_t c)
{
    return c < 0x000d800 || (c > 0x0000dfff && c <= 0x0010ffff);
}

#ifdef __cplusplus
}
#endif
#endif


