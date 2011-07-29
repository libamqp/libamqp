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

#ifndef LIBAMQP_THREAD_CONDITION_VARIABLE_H
#define LIBAMQP_THREAD_CONDITION_VARIABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Thread/Mutex.h"

typedef struct amqp_condition_variable_t
{
#if defined(AMQP__WIN32_THREADS)
    #error "no pthreads"
#else
    pthread_cond_t cv;
#endif
} amqp_condition_variable_t;

extern void amqp_condition_initialize(amqp_condition_variable_t *cv);
extern void amqp_condition_destroy(amqp_condition_variable_t *cv);
extern void amqp_condition_wait(amqp_condition_variable_t *cv, amqp_mutex_t *mutex);
extern void amqp_condition_notify(amqp_condition_variable_t *cv);
extern void amqp_condition_notify_all(amqp_condition_variable_t *cv);

#ifdef __cplusplus
}
#endif
#endif
