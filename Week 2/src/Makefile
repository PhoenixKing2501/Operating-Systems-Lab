run: shell

shell: shell.o history.o
	gcc -o shell shell.o history.o -Wall -std=c17 -fdiagnostics-color -D_GNU_SOURCE -lreadline

shell.o: shell.c history.h
	gcc -c shell.c -Wall -std=c17 -fdiagnostics-color -D_GNU_SOURCE -lreadline -I.

history.o: history.c history.h
	gcc -c history.c -Wall -std=c17 -fdiagnostics-color -D_GNU_SOURCE -lreadline -I.

clean:
	-rm shell shell.o history.o
