#!/bin/bash

# ensure libev in on the LD_LIBRARY_PATH
if [ "$(uname -a | awk '{print $2}')" = "ubuntu" ]
then
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib
fi

# remove artifacts from a prior build
rm -rf output
mkdir output
cd src
make -f Makefile.util scrub 

# Build everything and generate test report in XML for the xUnit plugin.
make TEST_RUNNER_ARGS=--xml=../output/test-result.xml ci-build JENKINS_BUILD_OPTIONS=-DJENKINS_BUILD

