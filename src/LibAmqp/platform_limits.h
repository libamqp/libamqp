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

#ifndef LIBAMQP_LIMITS_H
#define LIBAMQP_LIMITS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>

#ifndef CHAR_BIT
/* Danger, Will Robinson. 8 bits per byte is almost always correct. Almost always! */
#define CHAR_BIT    8
#endif

#ifndef LONG_BIT
#define LONG_BIT (sizeof(long) * CHAR_BIT)
#endif

#ifdef __cplusplus
}
#endif
#endif


