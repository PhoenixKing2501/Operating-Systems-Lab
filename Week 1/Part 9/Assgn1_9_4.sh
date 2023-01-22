res=$(gawk '{arr1[$1]++;arr2[$2]++}END{for(ele in arr2)l=l ele" "arr2[ele]"\n";for(ele in arr1)if(arr1[ele]==1)cnt++;else s=s""ele"\n";print l|"sort -k2,2nr>out.txt";print s""cnt}' $1)
cat out.txt
echo $res|sed 's/ /\n/g'
rm out.txt