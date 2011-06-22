#!/bin/bash

exec > decode-demo.out.txt

find TestData -type f -print | grep -v .DS_Store | grep -v decode-demo | 
while read data_file 
do 
    echo $data_file 
    src/decode-demo < "$data_file"

    echo
    echo
done

