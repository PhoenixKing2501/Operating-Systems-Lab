#! /bin/sh
while getopts "c:n:s:h:" arg; do
  case $arg in
    c) awk -F"," '$3 == $OPTARG {sum+=$2} END{print sum"\n"}'
  esac
  case $arg in
    n) awk -F"," '$4 == $OPTARG {sum+=$2} END{print sum"\n"}'
  esac
  case $arg in
    s) sort -k$OPTARG -n -t, "./main.csv"
  esac
  case $arg in
    h) Name1=$OPTARG;
  esac
done

echo "Hello $Name $Name1!"