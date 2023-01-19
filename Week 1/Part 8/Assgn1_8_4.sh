#! /bin/bash
[ ! -f "./main.csv" ] && echo "DATE,CATEGORY,AMOUNT,NAME" > main.csv; # file.txt will come at the end of the script
i=1
for ((i=1; i<=$#; i++))
do
if [ "${!i}" = "-c" -o "${!i}" = "-n" -o "${!i}" = "-s" ]; then
    ((i+=1))
elif [ "${!i}" != "-h" ];then
    break
fi
done

if [ ${i} -le $# ];then
  for ((j=0; j<=2; j++,i++))
  do
   s="${s}${!i},"
  done
   s="${s}${!i}"
   echo "$s" >> main.csv
fi

# { head -n 1 "./main.csv" && tail -n +2 "./main.csv" | sort -t, -k1.7,1.10n -k1.4,1.5n -k1.1,1.2n; } | tee "./main.csv"

var=`cat main.csv`
echo "$var"|( head -n 1  && tail -n +2 | sort -t, -k1.7,1.10n -k1.4,1.5n -k1.1,1.2n; ) > "./main.csv"

while getopts "c:n:s:h" arg; do
  case $arg in
    c) awk -F"," '$3 == $OPTARG {sum+=$2} END{print sum"\n"}';;
    n) awk -F"," '$4 == $OPTARG {sum+=$2} END{print sum"\n"}';;
    s) [$OPTARG -eq "Category"] &&  sort -k2 -t, "./main.csv";
        [$OPTARG -eq "Name"] &&  sort -k4 -t, "./main.csv";
        [$OPTARG -eq "Amount"] &&  sort -k3 -n -t, "./main.csv";;
    h) echo "helpful csv file";;
    *) echo "Invalid option";;
  esac
done
