#! /bin/bash

function gcd() {
	local a=$1
	local b=$2
	local t
	while [[ $b > 0 ]]; do
		let t=a
		let a=b
		let b=$((t % b))
	done
	echo $a
}

res=1

for i in $(rev $1); do
	res=$(($res / $(gcd $i $res) * $i))
done

echo $res
