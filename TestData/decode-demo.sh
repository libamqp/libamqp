#!/bin/bash
 > decode-demo.out.txt

find . -type f -print | grep -v .DS_Store | grep -v decode-demo | 
while read line 
do 
    echo $line > decode-demo.out.txt
    ./decode-demo < $line >> decode-demo.out.txt

    echo
    echo
done

