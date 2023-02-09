run: Assignment2_4_20CS30057_20CS30064_20CS30043_20CS30062 virus lock

Assignment2_4_20CS30057_20CS30064_20CS30043_20CS30062: Assignment2_4_20CS30057_20CS30064_20CS30043_20CS30062.o history.o delep.o sb.o
	gcc -o Assignment2_4_20CS30057_20CS30064_20CS30043_20CS30062 Assignment2_4_20CS30057_20CS30064_20CS30043_20CS30062.o history.o delep.o sb.o -O6 -Wall -std=c17 -fdiagnostics-color -D_GNU_SOURCE -lreadline -lm

Assignment2_4_20CS30057_20CS30064_20CS30043_20CS30062.o: Assignment2_4_20CS30057_20CS30064_20CS30043_20CS30062.c history.h
	gcc -c Assignment2_4_20CS30057_20CS30064_20CS30043_20CS30062.c -Wall -std=c17 -O6 -fdiagnostics-color -D_GNU_SOURCE -lreadline -lm -I.

history.o: history.c history.h
	gcc -c history.c -Wall -std=c17 -O6 -fdiagnostics-color -D_GNU_SOURCE -lreadline -lm -I.

delep.o: delep.c delep.h
	gcc -c delep.c -Wall -std=c17 -O6 -fdiagnostics-color -D_GNU_SOURCE -lreadline -lm -I.

sb.o: sb.c sb.h
	gcc -c sb.c -Wall -std=c17 -O6 -fdiagnostics-color -D_GNU_SOURCE -lreadline -lm -I.

lock: lock.c
	gcc -o lock lock.c -Wall -std=c17 -O6 -fdiagnostics-color -D_GNU_SOURCE

virus: virus.c
	gcc -o virus virus.c -Wall -std=c17 -O6 -fdiagnostics-color -D_GNU_SOURCE

clean:
	-rm Assignment2_4_20CS30057_20CS30064_20CS30043_20CS30062 Assignment2_4_20CS30057_20CS30064_20CS30043_20CS30062.o history.o delep.o sb.o lock virus
