#! /bin/sh


filter="[ "
header=""
if [ $# -gt 2 ]; then
    for((i=3;i<=$#;++i)); do
        filter=$filter".${!i}, "
        header=$header"${!i},"
    done
    filter=${filter:0:-2}
    header=${header:0:-1}
fi
filter=$filter" ] | @csv"

echo $filter
j=0
for entry in `ls "$1"`; do
    if [ -f "$1/$entry" ] && [ ${entry##*.} == "jsonl" ]; then
        # echo $entry
        # echo ${entry%.*}
        # touch "$2/${entry%.*}.csv"
        echo $header > "$2/${entry%.*}.csv"
        jq --raw-output "$filter" "$1/$entry" >> "$2/${entry%.*}.csv"
        j=$j+1
    fi
done




