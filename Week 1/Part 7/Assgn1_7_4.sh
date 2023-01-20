#! /bin/sh

chars=({A..Z})

mkdir -p "$2"
for ((i = 0; i < 26; i++)); do
	touch "$2/${chars[i]}.txt"
done

for ((i = 0; i < 26; i++)); do
	grep -hori "^${chars[i]}.*" "$1" | sort >"$2/${chars[i]}.txt"
done
