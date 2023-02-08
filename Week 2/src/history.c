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

void refresh_history()
{
	H.curr_index = H.h_end;
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

		H.cmd[H.h_end] = strdup("");

		history_save();
	}
}

void history_save()
{
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

void history_print()
{
	int cnt = 1;
	// cmd[h_end] is always empty string, the empty prompt
	for (int i = H.h_st; i != H.h_end; i = (i + 1) % (HISTORY_SIZE + 1))
	{
		printf("%5d : %s\n", cnt, H.cmd[i]);
		cnt++;
	}
}

int up_key_handler(int count, int key)
{
	if (H.curr_index != H.h_st)
		H.curr_index = (H.curr_index + HISTORY_SIZE) %
					   (HISTORY_SIZE + 1);

	rl_replace_line(H.cmd[H.curr_index], 0);
	rl_redisplay();
	rl_point = rl_end;
	return 0;
}

int down_key_handler(int count, int key)
{
	if (H.curr_index != H.h_end)
		H.curr_index = (H.curr_index + 1) %
					   (HISTORY_SIZE + 1);

	rl_replace_line(H.cmd[H.curr_index], 0);
	rl_redisplay();
	rl_point = rl_end;
	return 0;
}
