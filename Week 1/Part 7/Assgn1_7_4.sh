mkdir -p "$2"
for x in {A..Z}
do grep -hori "^${x}.*" "$1" | sort -fo "$2/${x}.txt"
done