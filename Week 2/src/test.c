#include <stdio.h>
#include <stdlib.h>

int main()
{
	char *line = NULL;
	size_t bufsize = 0;
	getdelim(&line, &bufsize, EOF, stdin);
	printf("%s", line);
	free(line);
}
