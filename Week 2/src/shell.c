#include <ctype.h>
#include <glob.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <readline/readline.h>

#define MAX_CMDS (1 << 6)
#define MAX_ARGS (1 << 6)

char *infile, *outfile;
bool bg, is_pipe_begin, is_pipe_end;
pid_t pid = -1;

int old_pipefd[2] = {STDIN_FILENO}, new_pipefd[2];

void handle_sigint(int sig)
{
	if (pid > 0)
	{
		kill(pid, SIGINT);
		pid = -2;
	}
	else
	{
		printf("\n\n> ");
		fflush(stdin);
		fflush(stdout);
	}
}

// void handle_sigtstp(int sig)
// {
// 	bg = true;
// 	pid = -1;
// }

void init_readline()
{
	rl_bind_keyseq("\\C-p", rl_get_previous_history);
	rl_bind_keyseq("\\C-n", rl_get_next_history);
}

char *shell_read_line()
{
	char *line = NULL;
	// size_t bufsize = 0;

	// if (getline(&line, &bufsize, stdin) == -1)
	// {
	// 	if (feof(stdin))
	// 	{
	// 		puts("exit");
	// 		exit(EXIT_SUCCESS);
	// 	}
	// 	else
	// 	{
	// 		perror("shell: getline");
	// 		exit(EXIT_FAILURE);
	// 	}
	// }

	fflush(stdin);
	line = readline("\n> ");

	if (line == NULL)
	{
		puts("exit");
		exit(EXIT_SUCCESS);
	}

	return line;
}

char **get_cmds(char *line)
{
	char **cmds = malloc(MAX_CMDS * sizeof(char *));
	char *cmd;
	int position = 0;

	cmd = strtok(line, "|");

	while (cmd != NULL)
	{
		cmds[position] = cmd;
		position++;

		cmd = strtok(NULL, "|");
	}
	cmds[position] = NULL;

	return cmds;
}

char **cmd_input_parse(char *line)
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
		   strcmp(arg, "&") != 0)
	{
		if (*arg == '\'' || *arg == '\"')
		{
			if (arg[strlen(arg) - 1] != *arg)
			{
				char *temp = strtok(NULL, " \t\n\r");
				while (temp != NULL)
				{
					*(temp - 1) = ' ';

					if (temp[strlen(temp) - 1] == *arg)
					{
						break;
					}

					temp = strtok(NULL, " \t\n\r");
				}
			}

			// *strchr(arg + 1, *arg) = '\0';
			arg[strlen(arg) - 1] = 0;
			arg++;
		}

		args[position] = arg;
		position++;

		arg = strtok(NULL, " \t\n\r");
	}
	args[position] = NULL;

	while (arg != NULL)
	{
		if (strcmp(arg, "<") == 0)
		{
			infile = strtok(NULL, " \t\r\n");
		}
		else if (strcmp(arg, ">") == 0)
		{
			outfile = strtok(NULL, " \t\r\n");
		}
		else if (strcmp(arg, "&") == 0)
		{
			bg = true;
		}

		arg = strtok(NULL, " \t\n\r");
	}

	return args;
}

char **expand_args(char **args)
{
	glob_t globbuf;
	char **newargs = malloc(MAX_ARGS * sizeof(char *));
	int i = 1, j = 1;
	newargs[0] = strdup(args[0]);

	while (args[i] != NULL)
	{
		if (glob(args[i], GLOB_MARK | GLOB_TILDE | GLOB_BRACE, NULL, &globbuf) == 0)
		{
			for (int k = 0; k < globbuf.gl_pathc; k++)
			{
				newargs[j] = strdup(globbuf.gl_pathv[k]);
				j++;
			}
			globfree(&globbuf);
		}
		else
		{
			newargs[j] = strdup(args[i]);
			j++;
		}
		i++;
	}
	newargs[j] = NULL;
	free(args);

	return newargs;
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
	else if (strcmp(args[0], "cd") == 0)
	{
		if (args[1] == NULL)
		{
			fprintf(stderr, "shell: expected argument to \"cd\"\n");
		}
		else
		{
			if (chdir(args[1]) != 0)
			{
				perror("shell");
			}
		}
		return 1;
	}

	pid = -1;
	int status;

	if (!is_pipe_end && pipe(new_pipefd) == -1)
	{
		perror("pipe");
		return 0;
	}

	pid = fork();
	if (pid == 0)
	{
		if (infile != NULL)
		{
			if (freopen(infile, "r", stdin) == NULL)
			{
				perror("shell");
				return 0;
			}
		}
		if (outfile != NULL)
		{
			freopen(outfile, "w", stdout);
		}

		if (!is_pipe_begin)
		{
			close(old_pipefd[1]);
			if (dup2(old_pipefd[0], STDIN_FILENO) == -1)
			{
				perror("dup2");
				return 0;
			}
		}

		if (!is_pipe_end)
		{
			close(new_pipefd[0]);
			if (dup2(new_pipefd[1], STDOUT_FILENO) == -1)
			{
				perror("dup2");
				return 0;
			}
		}

		if (execvp((strcmp(args[0], "cls") == 0
						? "clear"
						: args[0]),
				   args) == -1)
		{
			perror("shell");
		}
		return 0;
	}
	else if (pid < 0)
	{
		perror("shell");
	}
	else
	{
		if (!is_pipe_begin)
		{
			close(old_pipefd[0]);
		}

		old_pipefd[0] = new_pipefd[0];
		old_pipefd[1] = new_pipefd[1];

		if (!is_pipe_end)
		{
			close(new_pipefd[1]);
		}

		do
		{
			if (bg)
				break;
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

		if (WIFEXITED(status))
		{
			pid = -1;
		}
		else if (WIFSIGNALED(status))
		{
			pid = -2;
		}
	}

	return 1;
}

int main()
{
	char *line, *beg;
	char **cmds, **args;
	int status = 1;

	signal(SIGINT, handle_sigint);
	// signal(SIGTSTP, handle_sigtstp);

	// init_readline();

	do
	{
		line = shell_read_line();
		beg = line;

		while (isspace(*beg))
		{
			beg++;
			if (*beg == '\n')
			{
				free(line);
				line = NULL;
				break;
			}
		}

		if (line == NULL)
		{
			continue;
		}

		cmds = get_cmds(beg);

		is_pipe_begin = true;
		is_pipe_end = false;
		pid = -1;
		for (int i = 0; cmds[i] != NULL && status && pid != -2; i++)
		{
			if (cmds[i + 1] == NULL)
			{
				is_pipe_end = true;
			}

			args = cmd_input_parse(cmds[i]);

			if (args != NULL)
			{
				args = expand_args(args);
				status = shell_execute(args);

				for (int i = 0; args[i] != NULL; i++)
				{
					free(args[i]);
				}
				free(args);
			}

			if (i == 0)
			{
				is_pipe_begin = false;
			}
		}

		free(line);
		free(cmds);
	} while (status);

	return EXIT_SUCCESS;
}
