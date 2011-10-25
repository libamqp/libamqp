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

#ifndef LIBAMQP_TODO_H
#define LIBAMQP_TODO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdio.h>

#define _TODO(what)  (0)
#define TODO(what)   {printf("%s:%d: todo - %s\n", __FILE__, __LINE__, #what); abort();}

#define unsupported_operation(what)  TODO(unsupported_operation_ ## what)
#define not_implemented(what)  TODO(not_implemented___ ## what ## ___)
#define illegal_program_state(what)  TODO(illegal_program_state___ ## what ## ___)

#ifdef __cplusplus
}
#endif
#endif

