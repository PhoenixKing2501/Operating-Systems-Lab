awk '{print $2}' $1|sort|uniq -c|sort -k1,1nr -k2,2|awk '{print $2 " " $1}'
awk '{print $1}' $1|sort|uniq -c|sort -nr|awk '$1>1{print $2}$1==1{cnt++}END{print cnt}'