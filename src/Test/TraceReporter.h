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

#ifndef LIBAMQP_TEST_TRACE_REPORTER
#define LIBAMQP_TEST_TRACE_REPORTER

#include <TestReporter.h>

class TraceReporter : public UnitTest::TestReporter
{
private:
    virtual void ReportTestStart(UnitTest::TestDetails const& test);
    virtual void ReportFailure(UnitTest::TestDetails const& test, char const* failure);
    virtual void ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed);
    virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);
};

#endif