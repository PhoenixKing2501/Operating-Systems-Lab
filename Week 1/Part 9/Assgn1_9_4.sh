# res=$(gawk '{arr1[$1]++;arr2[$2]++}END{for(ele in arr2)ele" "arr2[ele]"\n">>"out.txt";for(ele in arr1)if(arr1[ele]==1)cnt++;else s=s""ele"\n";print s"\n"cnt}' $1)
# #cat out.txt|sort -k2,2nr
# echo $res
# #rm out.txt


# gawk '{
#                 arr1[$1]++;
#                 arr2[$2]++
#       }
#       END {
#                 #PROCINFO["sorted_in"] = "@val_num_asc"
#                 n=asorti(arr2,dest1);
#                 n=asort(arr2,dest2);
#                 i=0;
#                 for (i=1;i<=NR;i++)
#                 {
#                   print dest[i]" "arr2[dest[i]] 

#                   if(arr1[i]==1)
#                     cnt++;
#                   else 
#                     s=s""ele"\n";
#                     i++;
#                 }
#                    print s""cnt
                  
#         }
#         ' $1

# gawk '{
#                 arr1[$1]++;
#                 arr2[$2]++
#       }
#       END {
#                 for (ele in arr2)
#                 {
#                   print ele" "arr2[ele] | "sort -k2,2nr"
#                 }
#                 for (ele in arr1) 
#                     {
#                         if(arr1[ele]==1)
#                             cnt++;
#                         else 
#                             s=s""ele"\n";
#                     }
                   
#                    print s""cnt
                  
#         }
#         ' $1

# res=$(gawk '{arr1[$1]++;arr2[$2]++}END{for(ele in arr2)l=l ele" "arr2[ele]"\n";for(ele in arr1)if(arr1[ele]==1)cnt++;else s=s""ele"\n";print l|"sort -k2,2nr>out.txt";print s""cnt}' $1)
# cat out.txt
# echo $res|sed 's/ /\n/g'
# rm out.txt