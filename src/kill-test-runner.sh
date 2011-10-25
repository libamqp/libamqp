#!/bin/bash

kill_args="$@"
ps -ef | grep test-runner | egrep -v '(grep|kill)' | 
while read uid pid rest
do
    echo Killing $pid
    kill ${kill_args} ${pid}
done

