black=$(sed -zEe 's/\s/|/gm' fruits.txt)
sed usernames.txt -Ee "/^(.*(${black:0:(-1)}).*|.{1,4}|.{21,})$/I! s/^[a-z][a-z0-9]*[0-9]+[a-z0-9]*$/YES/gmi" -e 't' -Ee "s/.*/NO/gmi" >validation_results.txt