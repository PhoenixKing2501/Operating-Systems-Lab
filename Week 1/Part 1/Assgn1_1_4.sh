#! /bin/sh

fn=0
res=1

gcd() {
	local a=$1
	local b=$2
	local t
	while [[ $b > 0 ]]; do
		let t=a
		let a=b
		let b=$((t % b))
	done
	fn=$a
}

for i in $(rev $1); do
	gcd $i $res
	res=$((res / fn * i))
done

echo $res
