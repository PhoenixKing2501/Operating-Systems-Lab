#! /bin/bash
chars=({A..Z})
mkdir -p "$2"
for i in $(seq 0 25); do
	grep -hori "^${chars[i]}.*" "$1" | sort -fo "$2/${chars[i]}.txt"
done
