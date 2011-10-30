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

#ifndef LIBAMQP_DEBUG_HELPER_H
#define LIBAMQP_DEBUG_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include "todo.h"

// Helper function to mark breakpoints.
extern void break_one();
extern void break_two();

#define SOUTC(v)    printf("%s:%d: soutc: %s = %c\n" , __FILE__, __LINE__, #v, v);
#define SOUTV(v)    printf("%s:%d: soutv: %s = %ld\n" , __FILE__, __LINE__, #v, (long) v);
#define SOUTVP(v)   printf("%s:%d: soutvp: %s = %p\n" , __FILE__, __LINE__, #v, (void *) v);
#define SOUTVX(v)   printf("%s:%d: soutvx: %s = %lx\n" , __FILE__, __LINE__, #v, (long) v);
#define SOUTVF(v)   printf("%s:%d: soutvf: %s = %f\n" , __FILE__, __LINE__, #v, (double) v);
#define SOUTS(v)    printf("%s:%d: souts: %s\n", __FILE__, __LINE__, v);

#ifndef JENKINS_BUILD
#define RETURN_UNLESS_JENKINS() if (1) return;
#else
#define RETURN_UNLESS_JENKINS()
#endif

#ifdef __cplusplus
}
#endif
#endif

