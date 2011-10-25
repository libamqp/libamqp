#!/bin/bash

ext=.gcda
find . -name \*${ext} -print | grep -v "break_break" |
while read file_name
do
    d=$(dirname "$file_name")
    f=$(basename "$file_name" ${ext})
    echo gcov -o "${d}" "${d}/${f}.c"
     gcov -o "${d}" "${d}/${f}.c"
done
