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

void amqp_condition_initialize(amqp_condition_variable_t *cv)
{
#if defined(AMQP__WIN32_THREADS)
    cv->event_handle = CreateEvent(0, false, false, 0);
    cv->waiters_count = 0;
    amqp_mutex_initialize(&cv->internal_mutex);
#else
    pthread_cond_init(&cv->cv, 0);
#endif
}

void amqp_condition_destroy(amqp_condition_variable_t *cv)
{
#if defined(AMQP__WIN32_THREADS)
    CloseHandle(cv->event_handle);
    amqp_mutex_destroy(&cv->internal_mutex);
#else
    pthread_cond_destroy(&cv->cv);
#endif
}

void amqp_condition_wait(amqp_condition_variable_t *cv, amqp_mutex_t *mutex)
{
#if defined(AMQP__WIN32_THREADS)
    amqp_mutex_lock(&cv->internal_mutex);
    ++cv->waiters_count;
    amqp_mutex_unlock(&cv->internal_mutex);
    amqp_mutex_unlock(mutex);
    WaitForSingleObject(cv->event_handle, INFINITE);
    amqp_mutex_lock(&cv->internal_mutex);
    --cv->waiters_count;
    amqp_mutex_unlock(&cv->internal_mutex);
    amqp_mutex_lock(mutex);
#else
    pthread_cond_wait(&cv->cv, &mutex->mutex);
#endif
}

void amqp_condition_notify(amqp_condition_variable_t *cv)
{
#if defined(AMQP__WIN32_THREADS)
    amqp_mutex_lock(&cv->internal_mutex);
    if (cv->waiters_count > 0)
    {
        SetEvent(cv->event_handle);
    }
    amqp_mutex_unlock(&cv->internal_mutex);
#else
    pthread_cond_signal(&cv->cv);
#endif
}

void amqp_condition_broadcast(amqp_condition_variable_t *cv)
{
#if defined(AMQP__WIN32_THREADS)
    abort();
#else
    pthread_cond_broadcast(&cv->cv);
#endif
}
