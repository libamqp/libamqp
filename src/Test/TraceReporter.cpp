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

#include <stdio.h>

#include <TraceReporter.h>
#include <TestDetails.h>
#include <iostream>

using namespace UnitTest;

void TraceReporter::ReportFailure(TestDetails const& details, char const* failure)
{
#if defined(__APPLE__) || defined(__GNUG__)
    char const* const errorFormat = "%s:%d: error: Failure in %s: %s\n";
#else
    char const* const errorFormat = "%s(%d): error: Failure in %s: %s\n";
#endif

    using namespace std;
    printf(errorFormat, details.filename, details.lineNumber, details.testName, failure);
}

void TraceReporter::ReportTestStart(TestDetails const& details)
{
    using namespace std;
    printf("%s:%d: Starting test: %s::%s\n", details.filename, details.lineNumber, details.suiteName, details.testName);
}

void TraceReporter::ReportTestFinish(TestDetails const& details, float secondsElapsed)
{
    using namespace std;
    if (secondsElapsed > 0.001f)
    {
        printf("%s:%d: Finished test: %s::%s\n", details.filename, details.lineNumber, details.suiteName, details.testName);
    }
}

void TraceReporter::ReportSummary(int const totalTestCount, int const failedTestCount, int const failureCount, float secondsElapsed)
{
    using namespace std;
    if (failureCount > 0)
    {
        printf("FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
    }
    else
    {
        printf("Success: %d tests passed.\n", totalTestCount);
    }
    
    if (secondsElapsed > 0.001)
    {
        printf("Test time: %.3f seconds.\n", secondsElapsed);
    }
}
