[ ! -f "./main.csv" ] && echo "DATE,CATEGORY,AMOUNT,NAME" >main.csv # file.txt will come at the end of the script
i=1
for ((i = 1; i <= $#; i++)); do
	if [ "${!i}" = "-c" -o "${!i}" = "-n" -o "${!i}" = "-s" ]; then
		((i += 1))
	elif [ "${!i}" != "-h" ]; then
		break
	fi
done

if [ ${i} -le $# ]; then
	for ((j = 0; j <= 2; j++, i++)); do
		s="${s}${!i},"
	done
	s="${s}${!i}"
	echo "$s" >>main.csv
fi

# { head -n 1 "./main.csv" && tail -n +2 "./main.csv" | sort -t, -k1.7,1.10n -k1.4,1.5n -k1.1,1.2n; } | tee "./main.csv"
# var=$(cat main.csv)
# { echo "$var" | { head -n 1 && tail -n +2 | sort -t, -k1.7,1.10n -k1.4,1.5n -k1.1,1.2n; }; } >"./main.csv"

{ head -n1 "./main.csv" && tail -n+2 "./main.csv" | sort -t, -k1.7,1.10n -k1.4,1.5n -k1.1,1.2n; } >"./tmp.csv"
cat "./tmp.csv" >"./main.csv"
rm "./tmp.csv"

while getopts "c:n:s:h" arg; do
	case $arg in
	c) awk -v a="$OPTARG" -F"," '{if($2 == a)sum+=$3} END{printf("Amount spent in category %s is %f\n",a,sum)}' "./main.csv";;
	n) awk -v a="$OPTARG" -F"," '{if($4 == a)sum+=$3} END{printf("Amount spent by %s is %f\n",a,sum)}' "./main.csv";;
	s)  [ "$OPTARG" == "category" ] && { head -n1 "./main.csv" && tail -n+2 "./main.csv" | sort -t, -k2; }> "./tmp.csv"
		[ "$OPTARG" == "name" ] && { head -n1 "./main.csv" && tail -n+2 "./main.csv" | sort -t, -k4; }> "./tmp.csv"
		[ "$OPTARG" == "amount" ] && { head -n1 "./main.csv" && tail -n+2 "./main.csv" | sort -t, -k3; }> "./tmp.csv"
		[ "$OPTARG" == "date" ] && cat "./main.csv" > "./tmp.csv"
		cat "./tmp.csv" >"./main.csv"
		rm "./tmp.csv"
		;;
	h) echo -e "CSV FILE MANIPULATOR\nThis is an interface to store the expenses on various things such as games,sports and food on a trip\nThe csv should store the following columns\nDate (dd-mm-yy) | Category | Amount | Name\nAccepts a new argument of the form date,category,amount,name to insert to csv\noptions:\n-c category: accepts a category and prints the amount of money spend in that category\n-n name : accepts a name and print the amount spent by that person\n-s column: sort the csv by column name\n-h : display help prompt\n " ;;
	esac
done
