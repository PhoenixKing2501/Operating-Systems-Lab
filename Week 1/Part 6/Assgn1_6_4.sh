#! /bin/bash

N=1000000
declare -a isprime
isprime[1]=0


for (( i=2; i<=N; i++ ))
do
    isprime[i]=1
done
for(( i=2; i<=N; i++ ))
do
    if [ ${isprime[i]} -eq 1 ]
    then
        for(( j=$(( i * i )); j<=N; j=$(( j + i )) ))
        do
            isprime[j]=0
        done
    fi
done


echo "sieve done"

primes=" "
declare -a indices
indices[1]=0
for (( i=2; i<=N; i++ ))
do
    if [ ${isprime[i]} -eq 1 ]
    then
        primes+="$i "
        indices[i]=$(( ${#primes} - 1 ))
    else
        indices[i]=${indices[i-1]}
    fi
done

echo "primes list done"

cat input.txt | while read n
do
    echo ${primes:1:${indices[n]}}
done > output.txt

echo "all done"
