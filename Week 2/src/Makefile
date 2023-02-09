run: shell virus lock

shell: shell.o history.o delep.o sb.o
	gcc -o shell shell.o history.o delep.o sb.o -O6 -Wall -std=c17 -fdiagnostics-color -D_GNU_SOURCE -lreadline -lm

shell.o: shell.c history.h
	gcc -c shell.c -Wall -std=c17 -O6 -fdiagnostics-color -D_GNU_SOURCE -lreadline -lm -I.

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
	-rm shell shell.o history.o delep.o sb.o lock virus
