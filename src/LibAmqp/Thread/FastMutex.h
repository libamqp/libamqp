/*
Copyright (c) 2010 Marcus Geelnard

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/
/*
    A C version of TinyThread++ fast mutexes for libamqp.
*/
#ifndef _FAST_MUTEX_H_
#define _FAST_MUTEX_H_

/// @file

// Which platform are we on?
#if !defined(_TTHREAD_PLATFORM_DEFINED_)
  #if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    #define _TTHREAD_WIN32_
  #else
    #define _TTHREAD_POSIX_
  #endif
  #define _TTHREAD_PLATFORM_DEFINED_
#endif

// Check if we can support the assembly language level implementation (otherwise
// revert to the system API)
#if (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))) || \
    (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))) || \
    (defined(__GNUC__) && (defined(__ppc__)))
  #define _FAST_MUTEX_ASM_
#else
  #define _FAST_MUTEX_SYS_
#endif

#if defined(_TTHREAD_WIN32_)
  #include <windows.h>
#else
  #ifdef _FAST_MUTEX_ASM_
    #include <sched.h>
  #else
    #include <pthread.h>
  #endif
#endif


/// Fast mutex class.
/// This is a mutual exclusion object for synchronizing access to shared
/// memory areas for several threads. It is similar to the tthread::mutex class,
/// but instead of using system level functions, it is implemented as an atomic
/// spin lock with very low CPU overhead.
///
/// The \c fast_mutex class is NOT compatible with the \c condition_variable
/// class (however, it IS compatible with the \c lock_guard class). It should
/// also be noted that the \c fast_mutex class typically does not provide
/// as accurate thread scheduling as a the standard \c mutex class does.
///
/// Because of the limitations of the class, it should only be used in
/// situations where the mutex needs to be locked/unlocked very frequently.
///
/// @note The "fast" version of this class relies on inline assembler language,
/// which is currently only supported for 32/64-bit Intel x86/AMD64 and
/// PowerPC architectures on a limited number of compilers (GNU g++ and MS
/// Visual C++).
/// For other architectures/compilers, system functions are used instead.

#if defined(_FAST_MUTEX_ASM_)
#define AMQP__FAST_MUTEX_TYPE  int
#else
#define AMQP__FAST_MUTEX_TYPE  amqp_mutex_t
#endif
typedef AMQP__FAST_MUTEX_TYPE amqp_fast_mutex_t;


inline static void amqp_fast_mutex_initialize(amqp_fast_mutex_t *mutex)
{
#if defined(_FAST_MUTEX_ASM_)
    *mutex = 0;
#else
    amqp_mutex_initialize(mutex);
#endif
}

inline static void amqp_fast_mutex_destroy(amqp_fast_mutex_t *mutex)
{
#if !defined(_FAST_MUTEX_ASM_)
    amqp_mutex_destroy(mutex);
#endif
}

#if defined(_TTHREAD_WIN32_)
#define amqp__thread_yield()    Sleep(0)
#elif defined(_TTHREAD_POSIX_)
#define amqp__thread_yield()    sched_yield()
#endif

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#define amqp_lock_asm_from_tthread_lib(lock)    \
    asm volatile (                              \
        "movl $1,%%eax\n\t"                     \
        "xchg %%eax,%0\n\t"                     \
        "movl %%eax,%1\n\t"                     \
        : "=m" (*lock), "=m" (oldLock)          \
        :                                       \
        : "%eax", "memory"                      \
    );
#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#define amqp_lock_asm_from_tthread_lib(lock)    \
    __asm {                                     \
        mov eax,1                               \
        mov ecx,lock                            \
        xchg eax,[ecx]                          \
        mov oldLock,eax                         \
    }
#elif defined(__GNUC__) && (defined(__ppc__))
#define amqp_lock_asm_from_tthread_lib(lock)    \
    int newLock = 1;                            \
    asm volatile (                              \
        "\n1:\n\t"                              \
        "lwarx  %0,0,%1\n\t"                    \
        "cmpwi  0,%0,0\n\t"                     \
        "bne-   2f\n\t"                         \
        "stwcx. %2,0,%1\n\t"                    \
        "bne-   1b\n\t"                         \
        "isync\n"                               \
        "2:\n\t"                                \
        : "=&r" (oldLock)                       \
        : "r" (lock), "r" (newLock)             \
        : "cr0", "memory"                       \
    );
#endif

#if defined(_FAST_MUTEX_ASM_)
inline static int amqp_fast_mutex_try_lock(amqp_fast_mutex_t *mutex)
{
    int oldLock;
    amqp_lock_asm_from_tthread_lib(mutex);
    return (oldLock == 0);
}
#endif

inline static void amqp_fast_mutex_lock(amqp_fast_mutex_t *mutex)
{
#if defined(_FAST_MUTEX_ASM_)
    int gotLock;
    do {
        gotLock = amqp_fast_mutex_try_lock(mutex);
        if(!gotLock)
        {
            amqp__thread_yield();
        }
    } while(!gotLock);
#else
    amqp_mutex_lock(mutex);
#endif
}

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#define amqp_unlock_asm_from_tthread_lib(lock) \
    asm volatile (                              \
        "movl $0,%%eax\n\t"                     \
        "xchg %%eax,%0\n\t"                     \
        : "=m" (*lock)                          \
        :                                       \
        : "%eax", "memory"                      \
        );
#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#define amqp_unlock_asm_from_tthread_lib(lock)  \
    __asm {                                     \
        mov eax,0                               \
        mov ecx,lock                            \
        xchg eax,[ecx]                          \
    }
#elif defined(__GNUC__) && (defined(__ppc__))
// Replace sync with lwsync where possible?
#define amqp_unlock_asm_from_tthread_lib(mLock) \
    asm volatile (                              \
        "sync\n\t"                              \
        : : : "memory"                          \
    );                                          \
    (*mLock) = 0;
#endif

inline static void amqp_fast_mutex_unlock(amqp_fast_mutex_t *mutex)
{
#if defined(_FAST_MUTEX_ASM_)
    amqp_unlock_asm_from_tthread_lib(mutex);
#else
    amqp_mutex_unlock(mutex);
#endif
}

#endif // _FAST_MUTEX_H_
