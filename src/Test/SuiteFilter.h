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

#ifndef LIBAMQP_TEST_SUITE_FILTER_H
#define LIBAMQP_TEST_SUITE_FILTER_H

#include <TestHarness.h>
#include <vector>
#include <string.h>

class SuiteFilter
{
public:
    SuiteFilter();
    ~SuiteFilter();

    bool operator() (const UnitTest::Test * test) const;

    void add(const char *suite_name);
    void set_exclusion_mode();

private:
    std::vector<std::string> m_suites;
    bool m_is_exclusion_filter;
};

#endif