res=1
gcd(){ if [ "$2" -eq 0 ];then echo "$1"
else echo $(gcd $2 `expr $1 % $2`)
fi }
for i in $(rev $1)
do res=$((res / $(gcd $i $res) * i))
done
echo $res
