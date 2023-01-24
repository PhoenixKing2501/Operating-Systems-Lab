#! /bin/sh
[ ! -f "./main.csv" ] && echo "Date (dd-mm-yy),Category,Amount,Name" > main.csv; # file.txt will come at the end of the script
while [ True ]; do
if [ "$1" = "-c" -o "$1" = "-n" -o "$1" = "-s" ]; then
    shift 2
elif [ "$1" = "-h" ]; then
    shift 1
else
    break
fi
done

[ $# != 0 ] && echo $1","$2","$3","$4 >> main.csv