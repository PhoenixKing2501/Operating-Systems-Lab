#include "history.h"

history_t H;

void init_history()
{
	H.h_st = 0;

	FILE *fptr = fopen("history.txt", "r");
	if (fptr == NULL)
	{
		fptr = fopen("history.txt", "w");
		if (fptr == NULL)
		{
			printf("Error in opening history\n");
			exit(EXIT_FAILURE);
		}
		fclose(fptr);
		fptr = fopen("history.txt", "r");
	}

	H.h_end = 0;
	size_t cmdsize = 0;
	int nchars;
	while (true)
	{
		H.cmd[H.h_end] = NULL;
		nchars = getline(&(H.cmd[H.h_end]), &cmdsize, fptr);
		if (nchars == -1)
			break;
		H.cmd[H.h_end][nchars - 1] = '\0';
		H.h_end++;
	}
	H.curr_index = H.h_end;
	H.cmd[H.h_end] = strdup("");

	fclose(fptr);
}

void history_add(char *cmd)
{
	if (H.h_st == H.h_end ||
		strcmp(cmd, H.cmd[(H.h_end + HISTORY_SIZE) %
						  (HISTORY_SIZE + 1)]) != 0)
	{
		free(H.cmd[H.h_end]);
		H.cmd[H.h_end] = strdup(cmd);
		H.h_end = (H.h_end + 1) % (HISTORY_SIZE + 1);
		H.curr_index = H.h_end;

		if (H.h_end == H.h_st)
		{
			free(H.cmd[H.h_end]);
			H.h_st = (H.h_end + 1) % (HISTORY_SIZE + 1);
		}

		H.cmd[H.h_end] = strdup(""); // Do we need strdup() for this?

		FILE *fptr = fopen("history.txt", "w");
		if (fptr == NULL)
		{
			printf("Error in opening history\n");
			exit(EXIT_FAILURE);
		}

		for (int i = H.h_st; i != H.h_end; i = (i + 1) % (HISTORY_SIZE + 1))
		{
			fprintf(fptr, "%s\n", H.cmd[i]);
		}
		fclose(fptr);
	}
}

void history_print()
{
	int cnt = 1;
	for (int i = H.h_st; i != H.h_end; i = (i + 1) % (HISTORY_SIZE + 1)) // cmd[h_end] is always empty string, the empty prompt
	{
		printf("%5d : %s\n", cnt, H.cmd[i]);
		cnt++;
	}
}

int up_key_handler(int count, int key)
{
	if (H.curr_index != H.h_st)
		H.curr_index = (H.curr_index + HISTORY_SIZE) % (HISTORY_SIZE + 1);

	rl_replace_line(H.cmd[H.curr_index], 0);
	rl_redisplay();
	rl_point = rl_end;
	return 0;
}

int down_key_handler(int count, int key)
{
	if (H.curr_index != H.h_end)
		H.curr_index = (H.curr_index + 1) % (HISTORY_SIZE + 1);

	rl_replace_line(H.cmd[H.curr_index], 0);
	rl_redisplay();
	rl_point = rl_end;
	return 0;
}

// int main()
// {
// 	init_history(); // cmd[h_end] is always empty string for empty prompt

// 	history_print();

// 	rl_bind_keyseq("\\e[A", up_key_handler);
// 	rl_bind_keyseq("\\e[B", down_key_handler);

// 	char *line;
// 	while (true)
// 	{
// 		line = readline("> ");
// 		printf("line: %s (%lu)\n", line, strlen(line));
// 		if (strlen(line) > 0)
// 			history_add(line);
// 		free(line);
// 	}

// 	return 0;
// }
