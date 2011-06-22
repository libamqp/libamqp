#!/bin/sh
#
# See: http://aegis.sourceforge.net/auug97.pdf
#

find . -name module.mk -print | sed -e 's!^./!!' -e 's!/module.mk$!!' |
while read moduleDir
do
    echo ${moduleDir}
    FILE="${moduleDir}/module.mk"
    sed -e "s:^MODULE.*:MODULE = ${moduleDir}:" "${FILE}" > "${FILE}.sed.out" &&
    	mv ${FILE}.sed.out ${FILE}
done
    
