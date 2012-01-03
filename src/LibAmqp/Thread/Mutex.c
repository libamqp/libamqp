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

#include "libamqp_common.h"
#include "Thread/Thread.h"

void amqp_mutex_initialize(amqp_mutex_t *mutex)
{
#if defined(AMQP__WIN32_THREADS)
    InitializeCriticalSection(&mutex->mutex);
#else
    pthread_mutex_init(&mutex->mutex, 0);
#endif
}

void amqp_mutex_destroy(amqp_mutex_t *mutex)
{
#if defined(AMQP__WIN32_THREADS)
    DeleteCriticalSection(&mutex->mutex);
#else
    pthread_mutex_destroy(&mutex->mutex);
#endif
}

void amqp_mutex_lock(amqp_mutex_t *mutex)
{
#if defined(AMQP__WIN32_THREADS)
    EnterCriticalSection(&mutex->mutex);
#else
    pthread_mutex_lock(&mutex->mutex);
#endif
}

void amqp_mutex_unlock(amqp_mutex_t *mutex)
{
#if defined(AMQP__WIN32_THREADS)
    LeaveCriticalSection(&mutex->mutex);
#else
    pthread_mutex_unlock(&mutex->mutex);
#endif
}
