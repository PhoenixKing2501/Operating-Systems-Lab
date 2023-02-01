#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS (1 << 10)

char *infile, *outfile;
bool bg;

char *shell_read_line()
{
	char *line = NULL;
	size_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	return line;
}

char **shell_input_parse(char *line)
{
	char **args = malloc(MAX_ARGS * sizeof(char *));
	char *arg;
	int position = 0;
	infile = outfile = NULL;
	bg = false;

	arg = strtok(line, " \t\n\r");
	while (arg != NULL &&
		   strcmp(arg, "<") != 0 &&
		   strcmp(arg, ">") != 0 &&
		   strcmp(arg, "|") != 0 &&
		   strcmp(arg, "&") != 0)
	{
		args[position] = arg;
		position++;

		arg = strtok(NULL, " \t\n\r");
	}
	args[position] = NULL;

	while (arg != NULL)
	{
		if (strcmp(arg, "<") == 0)
		{
			infile = strtok(NULL, " \t\n\r");
		}
		else if (strcmp(arg, ">") == 0)
		{
			outfile = strtok(NULL, " \t\n\r");
		}
		// else if (strcmp(arg, "|") == 0)
		// {
		// 	cmd = args[0];
		// 	cmdargs = args;
		// }
		else if (strcmp(arg, "&") == 0)
		{
			bg = true;
		}

		arg = strtok(NULL, " \t\n\r");
	}

	return args;
}

int shell_execute(char **args)
{
	if (args[0] == NULL)
	{
		return 1;
	}

	if (strcmp(args[0], "exit") == 0)
	{
		return 0;
	}

	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0)
	{
		if (infile != NULL)
		{
			freopen(infile, "r", stdin);
		}
		if (outfile != NULL)
		{
			freopen(outfile, "w", stdout);
		}

		if (execvp(args[0], args) == -1)
		{
			perror("shell");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		perror("shell");
	}
	else
	{
		if (!bg)
		{
			do
			{
				waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	}

	return 1;
}

int main()
{
	char *line;
	char **args;
	int status;

	do
	{
		printf("> ");
		line = shell_read_line();
		args = shell_input_parse(line);
		status = shell_execute(args);

		free(line);
		free(args);
	} while (status);

	return EXIT_SUCCESS;
}
