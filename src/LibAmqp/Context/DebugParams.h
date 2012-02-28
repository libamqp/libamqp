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

#ifndef LIBAMQP_CONTEXT_DEBUG_PARAMS_H
#define LIBAMQP_CONTEXT_DEBUG_PARAMS_H

#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(LIBAMQP_DEBUG_ALLOCATIONS) || defined(LIBAMQP_DEBUG_DOUBLE_FREE)
#define AMQP_DEBUG_PARAMS       ,__FILE__,__LINE__
#define AMQP_DEBUG_PARAMS_DECL  ,const char *file_name,int line_number
#define AMQP_DEBUG_ARGS         ,file_name,line_number
#else
#define AMQP_DEBUG_PARAMS
#define AMQP_DEBUG_PARAMS_DECL
#define AMQP_DEBUG_ARGS
#endif

#ifdef __cplusplus
}
#endif
#endif
