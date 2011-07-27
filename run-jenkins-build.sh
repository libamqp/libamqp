#!/bin/bash

# remove artifacts from a prior build
rm -rf output
mkdir output
cd src
make -f Makefile.util scrub 

# Build everything and generate test report in XML for the xUnit plugin.
make TEST_RUNNER_ARGS=--xml=../output/test-result.xml ci-build

