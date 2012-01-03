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

#include <SuiteFilter.h>

SuiteFilter::SuiteFilter() {}
SuiteFilter::~SuiteFilter() {}

bool SuiteFilter::operator() (const UnitTest::Test * test) const
{
    // Run all suites if no suite name was provided on the command line.
    if (m_suites.size() == 0) return true;

    for (std::vector<std::string>::const_iterator it = m_suites.begin(); it < m_suites.end(); it++)
    {
        if (*it == test->m_details.suiteName)
        {
            return !m_is_exclusion_filter;
        }
    }

    return m_is_exclusion_filter;
}

void SuiteFilter::add(const char *suite_name)
{
    std::string s(suite_name);
    m_suites.push_back(s);
}

void SuiteFilter::set_exclusion_mode()
{
    m_is_exclusion_filter = true;
}
