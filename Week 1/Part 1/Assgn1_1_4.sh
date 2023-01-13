#! /bin/sh

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

function lcm() {
	echo $(($1 / $(gcd $1 $2) * $2))
}

res=1

for i in $(rev $1); do
	res=$(lcm $i $res)
done

echo $res
