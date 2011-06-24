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

#ifndef LIBAMQP_TEST_EXTRA_CHECKS_H
#define LIBAMQP_TEST_EXTRA_CHECKS_H

#include <CheckMacros.h>

#ifdef CHECK_NULL
    #error UnitTest++ redefines CHECK_NULL
#endif

#ifdef CHECK_NOT_NULL
    #error UnitTest++ redefines CHECK_NOT_NULL
#endif

#define CHECK_NULL(value) \
    do \
    { \
        try { \
            if (!UnitTest::Check((void *) 0 == value)) \
                UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), #value); \
        } \
        catch (...) { \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), \
                    "Unhandled exception in CHECK_NULL(" #value ")"); \
        } \
    } while (0)

#define CHECK_NOT_NULL(value) \
    do \
    { \
        try { \
            if (UnitTest::Check((void *) 0 == value)) \
                UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), #value); \
        } \
        catch (...) { \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), \
                    "Unhandled exception in CHECK_NOT_NULL(" #value ")"); \
        } \
    } while (0)

#define CHECK_EQUAL_XX(expected, actual) \
    do \
    { \
        try { \
            UnitTest::CheckEqual(*UnitTest::CurrentTest::Results(), expected, actual, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__)); \
        } \
        catch (...) { \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __LINE__), \
                    "Unhandled exception in CHECK_EQUAL(" #expected ", " #actual ")"); \
        } \
    } while (0)


#endif