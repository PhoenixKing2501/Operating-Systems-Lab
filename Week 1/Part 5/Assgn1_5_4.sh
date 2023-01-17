#! /bin/sh
egrep -n '^[ ]*#.*|""".*"""' `find $* | grep "\.py$"`