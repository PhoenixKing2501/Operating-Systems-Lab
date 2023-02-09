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

#include "delep.h"
#include "history.h"
#include "sb.h"

// Max number of commands in a single line
#define MAX_CMDS (1 << 6)
// Max number of arguments in a single command
#define MAX_ARGS (1 << 6)
// Max number of background processes
#define MAX_BGPS (1 << 8)

// Global variables
char *infile, *outfile;
bool bg, is_pipe_begin, is_pipe_end;
pid_t pid = -1;
int old_pipefd[2] = {STDIN_FILENO}, new_pipefd[2];
int pid_index = 0, bg_index = 0;
pid_t pids[MAX_CMDS], bg_pids[MAX_BGPS];

// Function to handle SIGINT
void handle_sigint(int sig)
{
	if (pid < 0)
	{
		rl_delete_text(0, rl_end);
		printf("\n");
		rl_forced_update_display();
	}
}

// Function to handle SIGTSTP
void handle_sigtstp(int sig)
{
	bg = true;
	if (pid_index > 0)
	{
		for (int i = 0; i < pid_index; ++i)
		{
			bg_pids[bg_index++] = pids[i];
		}
	}

	for (int i = 0; i < bg_index; ++i)
	{
		kill(bg_pids[i], SIGCONT);
	}
}

// Function to handle Ctrl+A
int ctrl_a_handler(int count, int key)
{
	rl_point = 0;
	return 0;
}

// Function to handle Ctrl+E
int ctrl_e_handler(int count, int key)
{
	rl_point = rl_end;
	return 0;
}

// Function to handle left arrow key
int left_key_handler(int count, int key)
{
	if (rl_point > 0)
	{
		rl_point--;
	}
	return 0;
}

// Function to handle right arrow key
int right_key_handler(int count, int key)
{
	if (rl_point < rl_end)
	{
		rl_point++;
	}
	return 0;
}

// Function to init realine handlers
void init_readline()
{
	rl_bind_keyseq("\\e[A", up_key_handler);
	rl_bind_keyseq("\\e[B", down_key_handler);
	rl_bind_keyseq("\\e[C", right_key_handler);
	rl_bind_keyseq("\\e[D", left_key_handler);
	rl_bind_keyseq("\\C-a", ctrl_a_handler);
	rl_bind_keyseq("\\C-e", ctrl_e_handler);
}

// Function to read input from user
char *shell_read_line()
{
	refresh_history();
	char *line = readline("\n> ");

	if (line == NULL)
	{
		puts("exit");
		history_save();
		exit(EXIT_SUCCESS);
	}

	return line;
}

// Function to parse input
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

// Function to parse command
char **cmd_input_parse(char *line)
{
	char **args = malloc(MAX_ARGS * sizeof(char *));
	char *arg;
	int position = 0;
	infile = outfile = NULL;

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

// Function to expand wildcards
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

// Function to wait for child process
void child_wait(pid_t _pid)
{
	int status;
	pid_t ret;

	do
	{
		if (bg)
			break;

		ret = waitpid(_pid, &status, WNOHANG | WUNTRACED);

		if (ret == -1)
		{
			perror("waitpid");
			exit(EXIT_FAILURE);
		}

	} while (ret == 0 ||
			 (!WIFEXITED(status) &&
			  !WIFSIGNALED(status)));

	if (!bg)
	{
		if (WIFEXITED(status))
		{
			pid = -1;
		}
		else if (WIFSIGNALED(status))
		{
			pid = -2;
		}
	}
}

// Function to execute command
int shell_execute(char **args)
{
	if (args[0] == NULL)
	{
		return 1;
	}

	if (strcmp(args[0], "exit") == 0)
	{
		history_save();
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
	else if (strcmp(args[0], "history") == 0)
	{
		history_print();
		return 1;
	}
	else if (strcmp(args[0], "wait") == 0)
	{
		for (int i = 0; i < bg_index; ++i)
		{
			waitpid(bg_pids[i], NULL, 0);
			printf("Process %d exited\n", bg_pids[i]);
		}
		bg_index = 0;
		return 1;
	}
	else if (strcmp(args[0], "sb") == 0)
	{
		pid_t sb_pid = fork();

		if (sb_pid == 0)
		{
			int ret = squashbug(args);
			exit(ret);
		}
		else if (sb_pid < 0)
		{
			perror("shell");
		}
		else
		{
			int sb_status;
			waitpid(sb_pid, &sb_status, 0);

			return sb_status == 0;
		}
	}
	else if (strcmp(args[0], "delep") == 0)
	{
		if (args[1] == NULL)
		{
			fprintf(stderr, "shell: expected argument to \"delep\"\n");
			return 1;
		}

		int delep_pipefd[2];
		if (pipe(delep_pipefd) == -1)
		{
			perror("pipe");
			return 0;
		}

		pid_t delep_pid = fork();

		if (delep_pid == 0)
		{
			close(delep_pipefd[0]);
			int ret = delep_list_files(args[1], delep_pipefd[1]);
			close(delep_pipefd[1]);
			exit(ret);
		}
		else if (delep_pid < 0)
		{
			perror("shell");
		}
		else
		{
			close(delep_pipefd[1]);

			int unique_procs_to_kill = 0;
			int delep_status;
			pid_t procs_to_kill[MAX_PROCS_TO_KILL];

			read(delep_pipefd[0], &unique_procs_to_kill, sizeof(int));
			read(delep_pipefd[0], procs_to_kill, unique_procs_to_kill * sizeof(pid_t));

			close(delep_pipefd[0]);
			waitpid(delep_pid, &delep_status, 0);

			if (delep_status == -1)
			{
				return 0;
			}

			puts("Process IDs having file open:");
			for (int i = 0; i < unique_procs_to_kill; ++i)
			{
				printf("%d\n", abs(procs_to_kill[i]));
			}

			puts("Process IDs having file locked:");
			for (int i = 0; i < unique_procs_to_kill; ++i)
			{
				if (procs_to_kill[i] < 0)
				{
					printf("%d\n", -procs_to_kill[i]);
				}
			}

			if (unique_procs_to_kill > 0)
			{
				char *answer = NULL;
				size_t answer_size = 0;
				printf("Kill processes? [y/n] ");
				getline(&answer, &answer_size, stdin);
				if (tolower(answer[0]) == 'y')
				{
					for (int i = 0; i < unique_procs_to_kill; ++i)
					{
						kill(abs(procs_to_kill[i]), SIGKILL);
					}
				}
				free(answer);
			}

			return 1;
		}
	}

	pid = -1;

	if (!is_pipe_end &&
		pipe(new_pipefd) == -1)
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
				exit(EXIT_FAILURE);
			}
		}
		if (outfile != NULL)
		{
			if (freopen(outfile, "w", stdout) == NULL)
			{
				perror("shell");
				exit(EXIT_FAILURE);
			}
		}

		if (!is_pipe_begin)
		{
			close(old_pipefd[1]);
			if (dup2(old_pipefd[0], STDIN_FILENO) == -1)
			{
				perror("dup2");
				exit(EXIT_FAILURE);
			}
		}

		if (!is_pipe_end)
		{
			close(new_pipefd[0]);
			if (dup2(new_pipefd[1], STDOUT_FILENO) == -1)
			{
				perror("dup2");
				exit(EXIT_FAILURE);
			}
		}

		char *cmd = strcmp(args[0], "cls") == 0
						? "clear"
						: args[0];

		if (execvp(cmd, args) == -1)
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
		pids[pid_index++] = pid;

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
	}

	return 1;
}

int main()
{
	char *line, *beg;
	char **cmds, **args;
	bool status;

	signal(SIGINT, handle_sigint);
	signal(SIGTSTP, handle_sigtstp);

	init_readline();
	init_history();

	do
	{
		bg = false;
		is_pipe_begin = true;
		is_pipe_end = false;
		pid = -1;
		status = true;
		pid_index = 0;

		memset(pids, 0, sizeof(pids));

		line = shell_read_line();
		beg = line;

		while (isspace(*beg))
		{
			++beg;
			if (*beg == '\n')
			{
				free(line);
				line = NULL;
				break;
			}
		}

		if (line == NULL || *line == 0)
		{
			continue;
		}

		history_add(beg);
		cmds = get_cmds(beg);

		for (int i = 0; cmds[i] != NULL && status; ++i)
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

		for (int i = 0; i < pid_index; ++i)
		{
			child_wait(pids[i]);
		}

		free(line);
		free(cmds);
	} while (true);

	return EXIT_SUCCESS;
}
