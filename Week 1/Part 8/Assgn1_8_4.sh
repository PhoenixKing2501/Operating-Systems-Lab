#! /bin/sh
[ ! -f "./main.csv" ] && echo "Date (dd-mm-yy),Category,Amount,Name" > main.csv; # file.txt will come at the end of the script
i=1
while [ "$i" -le $# ];
do
if [ "$i" = "-c" -o "$i" = "-n" -o "$i" = "-s" ]; then
    ((i+=1))
elif [ "$i" != "-h" ];then
    break
fi
   ((i+=1))
done

[ "$i" != $# ] && echo "$i,${i+1},${i+2},${i+3}" >> main.csv

sort -t, -k1.7,1.10n -k1.4,1.5n -k1.1,1.2n "./main.csv"

while getopts "c:n:s:h:" arg; do
  case $arg in
    c) awk -F"," '$3 == $OPTARG {sum+=$2} END{print sum"\n"}'
  esac
  case $arg in
    n) awk -F"," '$4 == $OPTARG {sum+=$2} END{print sum"\n"}'
  esac
  case $arg in
    s) [$OPTARG -eq "Category"] &&  sort -k2 -t, "./main.csv";
        [$OPTARG -eq "Name"] &&  sort -k4 -t, "./main.csv";
        [$OPTARG -eq "Amount"] &&  sort -k3 -n -t, "./main.csv";
  esac
  case $arg in
    h) echo "helpful csv file"
  esac
done
