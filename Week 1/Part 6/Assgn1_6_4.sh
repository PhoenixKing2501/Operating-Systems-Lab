N=1000000
minfac=([1]=1)

for ((i = 2; i <= N; i++)); do
	minfac[i]=$i
done
for ((i = 2; i * i <= N; i++)); do
	if [ ${minfac[i]} -eq $i ]; then
		for ((j = i * i; j <= N; j += i)); do
			if [ $j -eq ${minfac[j]} ]; then
				minfac[j]=$i
			fi
		done
	fi
done

cat /dev/null >output.txt.gz

cat input.txt | while read n; do
	while [ $n -gt 1 ]; do
		echo -n "${minfac[n]} "
		n=$((n / ${minfac[n]}))
	done | gzip -c >>output.txt.gz
	echo | gzip -c >>output.txt.gz
done
