for pid in $(ps -eaf | awk ' {if ($8 == "./virus") print $2}'); do kill -9 $pid; done