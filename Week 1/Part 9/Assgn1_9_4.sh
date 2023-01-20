#! /bin/gawk -f
gawk '{ arr1[$1]++;arr2[$2]++ } END {for (ele in arr2){  print ele" "arr2[ele] | "sort -k2,2nr"}for (ele in arr1) {if(arr1[ele]==1)cnt++;else s=s""ele"\n";}print s""cnt  }' $1
