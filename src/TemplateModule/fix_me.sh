#!/bin/bash

if [ $# -eq 0 ]
then
    target=$(basename $(pwd))
    parent=$(basename $(dirname $(pwd)))
elif [ $# -eq 1 ]
then
    target=$1
    parent=$1
fi

function to_lowercase
{
    echo $1 | sed -e 's/\([a-z]\)\([A-Z]\)/\1_\2/g' \
	-e 'y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/' 
}
function to_camelcase
{
    echo $1 | sed -e 's/_/\
/' | sed -e 's/^\(.\)\(.*\)$/\1\
\2/' | sed -e  '/^.$/ y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/' |  tr -d '\n'
}
function to_uppercase
{
    echo $1 | sed -e  '/^./ y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/' 
}

lc_target=$(to_lowercase ${target})
cc_target=$(to_camelcase ${target})
uc_target=$(to_uppercase ${target})

lc_parent=$(to_lowercase ${parent})
cc_parent=$(to_camelcase ${parent})
uc_parent=$(to_uppercase ${parent})

if [ "${parent}" = "LibAmqp" ]; then
    echo Changing module to ${lc_target}, Module to ${cc_target} and MODULE to ${uc_target}
else
    echo Changing module to ${lc_target}, Module to ${cc_target} and MODULE to ${uc_target}, and
    echo Changing parent to ${lc_target}, Parent to ${cc_target} and PARENT to ${uc_target}
fi

for i in module.* Module*.*
do
    if [ "${parent}" = "LibAmqp" ]; then
        sed -e "s/Parent\///g" -e "s/Parent//g" -e "s/PARENT_//g" $i > $i.t && mv $i.t $i
    fi
    sed \
    	-e "s/module/${lc_target}/g" -e "s/Module/${cc_target}/g" -e "s/MODULE/${uc_target}/g" \
    	-e "s/parent/${lc_parent}/g" -e "s/Parent/${cc_parent}/g" -e "s/PARENT/${uc_parent}/g" \
    	-e "s/MAKEVAR/MODULE/g" \
	$i > $i.t && mv $i.t $i
done

mv Module.c ${cc_target}.c
mv Module.h ${cc_target}.h
mv ModuleTest.cpp ${cc_target}Test.cpp
mv ModuleTestSupport.cpp ${cc_target}TestSupport.cpp
mv ModuleTestSupport.h ${cc_target}TestSupport.h

[ -f fix_me.sh ] && rm -f README fix_me.sh
