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
/* Can't have or don't want fast mutexes */

#define amqp_fast_mutex_t void *
#define amqp_fast_mutex_initialize(m) amqp_mutex_initialize((amqp_mutex_t *) m)
#define amqp_fast_mutex_destroy(m) amqp_mutex_destroy((amqp_mutex_t *) m)
#define amqp_fast_mutex_lock(m) amqp_mutex_lock((amqp_mutex_t *) m)
#define amqp_fast_mutex_unlock(m) amqp_mutex_unlock((amqp_mutex_t *) m)
#else

typedef volatile int amqp_fast_mutex_t;

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
    int old_lock;
    amqp_lock_asm_from_tthread_lib(mutex);
    return (old_lock == 0);
}

inline static void amqp_fast_mutex_lock(amqp_fast_mutex_t *mutex)
{
    int got_lock;
    do {
        got_lock = amqp_fast_mutex_try_lock(mutex);
        if(!got_lock)
        {
            amqp__thread_yield();
        }
    } while(!got_lock);
}

inline static void amqp_fast_mutex_unlock(amqp_fast_mutex_t *mutex)
{
    amqp_unlock_asm_from_tthread_lib(mutex);
}

#endif /* !defined(_TTHREAD_FAST_MUTEX_ASM_) || defined (LIBAMQP_DISABLE_FAST_MUTEX) */
#endif /* LIBAMQP_THREAD_FAST_MUTEX_H */
