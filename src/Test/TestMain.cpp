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

#include <TestHarness.h>
#include <TestReporterStdout.h>
#include <TraceReporter.h>
#include <iostream>

//SUITE(TestSuite)
//{
//    class Fixture
//    {
//    public:
//        Fixture() {
//            std::cout << "Fixture ctor" << std::endl;
//        }
//
//        ~Fixture() {
//            std::cout << "Fixture dtor" << std::endl;
//        }
//    };
//
//    TEST_FIXTURE(Fixture, YourTestName)
//    {
//        CHECK(1);
//        sleep(2);
//    }
//}

int main(int, char const *[])
{
    TraceReporter reporter;
    UnitTest::TestReporterStdout stdoutReporter;
    
    UnitTest::TestRunner runner(stdoutReporter);
    return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);
}
