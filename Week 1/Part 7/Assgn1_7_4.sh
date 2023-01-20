#! /bin/sh

chars=( {a..z} )

mkdir -p "$2"
for (( i=0; i<26; i++ ))
do
    touch "$2/${chars[i]}.txt"
done

for (( i=0; i<26; i++ ))
do
    grep -horiw "${chars[i]}.*" "$1" | sort > "$2/${chars[i]}.txt"
done