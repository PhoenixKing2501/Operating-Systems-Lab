#! /bin/sh

sed -e "/$2/ {s/.*/\U&/;s/\([A-Z][^a-zA-Z]*\)\([a-zA-Z]\)/\1\L\2/g}"  $1

