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

#ifndef LIBAMQP_THREAD_THREAD_H
#define LIBAMQP_THREAD_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define AMQP__WIN32_THREADS 1
#include <windows.h>
#include "libamqp_win32.h"
#else
#include <pthread.h>
#include <unistd.h>
#endif

typedef void (*amqp_thread_handler_t)(void *argument);

typedef struct amqp_thread_t amqp_thread_t;

extern amqp_thread_t *amqp_thread_start(amqp_thread_handler_t handler, void *handler_argument);
extern void amqp_thread_destroy(amqp_thread_t *thread);
extern int amqp_thread_is_running(amqp_thread_t *thread);

#ifdef __cplusplus
}
#endif
#endif
