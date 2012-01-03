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

typedef struct amqp_mutex_t
{
#if defined(AMQP__WIN32_THREADS)
    CRITICAL_SECTION mutex;
#else
    pthread_mutex_t mutex;
#endif
} amqp_mutex_t;

typedef struct amqp_condition_variable_t
{
#if defined(AMQP__WIN32_THREADS)
    HANDLE event_handle;
	amqp_mutex_t internal_mutex;
    unsigned int waiters_count;
#else
    pthread_cond_t cv;
#endif
} amqp_condition_variable_t;

typedef struct
{
    amqp_mutex_t mutex;
    amqp_condition_variable_t cv;
    int count;
} amqp_semaphore_t;

typedef void (*amqp_thread_handler_t)(void *argument);

typedef struct amqp_thread_t amqp_thread_t;

extern amqp_thread_t *amqp_thread_start(amqp_thread_handler_t handler, void *handler_argument);
extern void amqp_thread_destroy(amqp_thread_t *thread);
extern int amqp_thread_is_running(amqp_thread_t *thread);

extern void amqp_mutex_initialize(amqp_mutex_t *mutex);
extern void amqp_mutex_destroy(amqp_mutex_t *mutex);
extern void amqp_mutex_lock(amqp_mutex_t *mutex);
extern void amqp_mutex_unlock(amqp_mutex_t *mutex);

extern void amqp_condition_initialize(amqp_condition_variable_t *cv);
extern void amqp_condition_destroy(amqp_condition_variable_t *cv);
extern void amqp_condition_wait(amqp_condition_variable_t *cv, amqp_mutex_t *mutex);
extern void amqp_condition_notify(amqp_condition_variable_t *cv);
extern void amqp_condition_broadcast(amqp_condition_variable_t *cv);

extern void amqp_semaphore_initialize(amqp_semaphore_t *semaphore);
extern void amqp_semaphore_destroy(amqp_semaphore_t *semaphore);
extern void amqp_semaphore_wait(amqp_semaphore_t *semaphore);
extern void amqp_semaphore_signal(amqp_semaphore_t *semaphore);

#ifdef __cplusplus
}
#endif
#endif
