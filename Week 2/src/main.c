// #include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void shell_loop(void)
{
	char *line;
	char **args;
	int status;

	do {
		printf("> ");
		line = shell_read_line();
		args = shell_split_line(line);
		status = shell_execute(args);

		free(line);
		free(args);
	} while (status);
}

int main()
{
	shell_loop();
	return EXIT_SUCCESS;
}
