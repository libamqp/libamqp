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

#include <TestHarness.h>
#include <TestReporterStdout.h>
#include <XmlTestReporter.h>
#include <TraceReporter.h>
#include <iostream>
#include <fstream>
#include <SuiteFilter.h>


class ReporterMinder
{
public:
    ReporterMinder(UnitTest::TestReporter *_reporter) : reporter(_reporter) {}
    ~ReporterMinder() { delete reporter; }
    UnitTest::TestReporter& theReporter() const { return *reporter; }

private:
    UnitTest::TestReporter *reporter;
};

class ArgumentProcessor
{
public:
    ArgumentProcessor(SuiteFilter& filter) : _filter(filter), _reporter(0)
    {
    }
    ~ArgumentProcessor()
    {
        delete _reporter;
    }
    UnitTest::TestReporter& theReporter() const { return *_reporter; }

    void scan(int argc, char const *argv[])
    {
        int i = 1;
        while (i < argc && argv[i][0] == '-')
        {
            if (strcmp("--trace", argv[i]) == 0 && _reporter == 0)
            {
                _reporter = new TraceReporter();
            }
            else if (strcmp("--xml", argv[i]) == 0 && _reporter == 0)
            {
                _reporter = new UnitTest::XmlTestReporter(std::cout);
            }
            else if (strncmp("--xml=", argv[i], strlen("--xml=")) == 0 && _reporter == 0)
            {
                _resultFile.open(&argv[i][strlen("--xml=")]);
                _reporter = new UnitTest::XmlTestReporter(_resultFile);
            }
            else if (strcmp("--exclude", argv[i]) == 0)
            {
                _filter.set_exclusion_mode();
            }
            i++;
        }

        while (i < argc)
        {
            _filter.add(argv[i++]);
        }
        
        if (_reporter == 0)
        {
            _reporter = new UnitTest::TestReporterStdout();
        }
    }

private:
    std::ofstream _resultFile;
    SuiteFilter& _filter;
    UnitTest::TestReporter *_reporter;
};

int main(int argc, char const *argv[])
{
    SuiteFilter filter;
    ArgumentProcessor argumentProcessor(filter);
    argumentProcessor.scan(argc, argv);

    UnitTest::TestRunner runner(argumentProcessor.theReporter());
    return runner.RunTestsIf(UnitTest::Test::GetTestList(), 0, filter, 0);
}
