sed -e "/\(^\|\s\)$2\($\|\s\)/ {s/.*/\U&/;s/\([A-Z][^a-zA-Z]*\)\([a-zA-Z]\)/\1\L\2/g}"  $1
