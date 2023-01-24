files=`find $1 | grep "\.py$"`
for file in $files;do
echo $file"\n"
awk '{ singleflag = 0;hashflag = 0;quoteflag = 0;}
        /""".*"""/{
            print "Line number : "NR" : "$0
            singleflag=1;
        }
        /"""/{
                if (multiflag == 0 && singleflag == 0) 
                    {
                        print "Line number : "NR
                        multiflag=1;
                    }
                else if(multiflag == 1 && singleflag == 0)
                    { 
                        print $0
                        multiflag=0; 
                    }
            }
            multiflag; 
            !mutliflag{
               for(i=1;i<=NF;i++)
                { 
                    if($i ~ /"/ && quoteflag ==0 && hashflag == 0)   # quote inside hash but not vice versa
                    {
                        quoteflag = 1;
                    }
                    else if($i=="#" && quoteflag == 0 && hashflag == 0)
                        {
                            printf("Line number : %d : ",NR);
                            hashflag=1;
                        } 
                    else if($i ~ /"/ && quoteflag == 1)
                        {
                            quoteflag = 0;
                        }
                    if(hashflag==1)
                        {
                            printf("%s ",$i);
                        }
                }
                if(hashflag==1)
                    {
                        print "\n";
                    }             
            }' $file
done