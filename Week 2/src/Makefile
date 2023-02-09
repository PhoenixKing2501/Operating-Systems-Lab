run: shell

shell: shell.o history.o delep.o sb.o
	gcc -o shell shell.o history.o delep.o sb.o -O -Wall -std=c17 -fdiagnostics-color -D_GNU_SOURCE -lreadline

shell.o: shell.c history.h
	gcc -c shell.c -Wall -std=c17 -O -fdiagnostics-color -D_GNU_SOURCE -lreadline -I.

history.o: history.c history.h
	gcc -c history.c -Wall -std=c17 -O -fdiagnostics-color -D_GNU_SOURCE -lreadline -I.

delep.o: delep.c delep.h
	gcc -c delep.c -Wall -std=c17 -O -fdiagnostics-color -D_GNU_SOURCE -lreadline -I.

sb.o: sb.c sb.h
	gcc -c sb.c -Wall -std=c17 -O -fdiagnostics-color -D_GNU_SOURCE -lreadline -I.

clean:
	-rm shell shell.o history.o delep.o sb.o
