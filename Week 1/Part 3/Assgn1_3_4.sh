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
filter=$filter" | tostring ] | @csv"

for entry in `ls "$1"`; do
    if [ -f "$1/$entry" ] && [ ${entry##*.} == "jsonl" ]; then
        echo $header > "$2/${entry%.*}.csv"
        jq -r "$filter" "$1/$entry" >> "$2/${entry%.*}.csv"
    fi
done
