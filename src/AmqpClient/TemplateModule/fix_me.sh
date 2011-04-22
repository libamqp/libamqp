#!/bin/bash

if [ $# -eq 0 ]
then
    target=$(basename $(pwd))
elif [ $# -eq 1 ]
then
    target=$1
fi

lc_target=$(echo ${target} | sed -e 's/\([a-z]\)\([A-Z]\)/\1_\2/g' \
	-e 'y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/' )

cc_target=$(echo ${lc_target} | sed -e 's/_/\
/' | sed -e 's/^\(.\)\(.*\)$/\1\
\2/' | sed -e  '/^.$/ y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/' |  tr -d '\n')

uc_target=$(echo ${lc_target} | sed -e  '/^./ y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/' )

echo Changing module to ${lc_target}, Module to ${cc_target} and MODULE to ${uc_target}

for i in module.* Module*.*
do
    sed -e "s/module/${lc_target}/g" -e "s/Module/${cc_target}/g" -e "s/MODULE/${uc_target}/g" $i > $i.t && mv $i.t $i
done

mv module.c ${lc_target}.c
mv module.h ${lc_target}.h
mv ModuleTest.cpp ${cc_target}Test.cpp
mv ModuleTestSupport.cpp ${cc_target}TestSupport.cpp
mv ModuleTestSupport.h ${cc_target}TestSupport.h

