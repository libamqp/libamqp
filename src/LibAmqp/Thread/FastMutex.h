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

/*
    A C version of the TinyThread++ fast mutexes implementation
*/
#ifndef LIBAMQP_THREAD_FAST_MUTEX_H
#define LIBAMQP_THREAD_FAST_MUTEX_H

#include "Thread/fast_mutex.h"
#include "Thread/Thread.h"

#if defined(_TTHREAD_WIN32_)
#define amqp__thread_yield()    Sleep(0)
#elif defined(_TTHREAD_POSIX_)
#define amqp__thread_yield()    sched_yield()
#endif

#if !defined(_TTHREAD_FAST_MUTEX_ASM_) || defined (LIBAMQP_DISABLE_FAST_MUTEX)
#define amqp_fast_mutex_t amqp_mutex_t
#define amqp_fast_mutex_initialize amqp_mutex_initialize
#define amqp_fast_mutex_destroy amqp_mutex_destroy
#define amqp_fast_mutex_lock amqp_mutex_lock
#define amqp_fast_mutex_unlock amqp_mutex_unlock
#else

/* Can't have or don't want fast mutexes */
typedef int amqp_fast_mutex_t;

inline static void amqp_fast_mutex_initialize(amqp_fast_mutex_t *mutex)
{
    *mutex = 0;
}

inline static void amqp_fast_mutex_destroy(amqp_fast_mutex_t *mutex)
{
    // nothing to do
}

inline static int amqp_fast_mutex_try_lock(amqp_fast_mutex_t *mutex)
{
    int oldLock;
    amqp_lock_asm_from_tthread_lib(mutex);
    return (oldLock == 0);
}

inline static void amqp_fast_mutex_lock(amqp_fast_mutex_t *mutex)
{
    int gotLock;
    do {
        gotLock = amqp_fast_mutex_try_lock(mutex);
        if(!gotLock)
        {
            amqp__thread_yield();
        }
    } while(!gotLock);
}

inline static void amqp_fast_mutex_unlock(amqp_fast_mutex_t *mutex)
{
    amqp_unlock_asm_from_tthread_lib(mutex);
}

#endif /* !defined(_TTHREAD_FAST_MUTEX_ASM_) || defined (LIBAMQP_DISABLE_FAST_MUTEX) */
#endif /* LIBAMQP_THREAD_FAST_MUTEX_H */