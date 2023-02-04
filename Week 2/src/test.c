// Just to test the shell with a simple program, We wrote this:

#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>

int main()
{
	char *line = NULL;
	size_t bufsize = 0;
	getdelim(&line, &bufsize, EOF, stdin);
	poll(NULL, 0, 5000);
	printf("%s", line);
	free(line);
}
