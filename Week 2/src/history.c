#include "history.h"

/*
 * history_t                    : data structure to store history of commands
 * underlying implementation    : circular array

 * a global variable            : history_t H
 * H.cmd[HISTORY_SIZE + 1]      : array, stores history of commands
 * H.h_st                       : int, start index of history (oldest) in circular array
 * H.h_end                      : int, end index of history (next to latest),
									always contains empty string, corresponding to empty prompt
 * H.curr_index                 : int, index of the command currently showing in the prompt
									(up and down arrow key changes the value of this variable)
 */

history_t H;

void init_history() // opens "history.txt" and loads into history buffer (to be called just after the shell starts)
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
	} // creates an empty "history.txt" if file not found

	H.h_end = 0;
	size_t cmdsize = 0;
	int nchars;
	while (H.h_end < HISTORY_SIZE)
	{
		H.cmd[H.h_end] = NULL;
		nchars = getline(&(H.cmd[H.h_end]), &cmdsize, fptr);
		if (nchars == -1)
			break;
		H.cmd[H.h_end][nchars - 1] = '\0';
		H.h_end++;
	}
	H.cmd[H.h_end] = strdup(""); // H.cmd[H.h_end] always contains empty string, corresponds to empty prompt
	H.curr_index = H.h_end;		 // bring the history pointer to the empty prompt

	fclose(fptr);
}

void refresh_history() // brings history pointer to empty prompt
{
	H.curr_index = H.h_end;
}

void history_add(char *cmd) // adds command to be executed to the history buffer
{
	if (H.h_st == H.h_end ||
		strcmp(cmd, H.cmd[(H.h_end + HISTORY_SIZE) %
						  (HISTORY_SIZE + 1)]) != 0)
	{
		free(H.cmd[H.h_end]);
		H.cmd[H.h_end] = strdup(cmd);
		H.h_end = (H.h_end + 1) % (HISTORY_SIZE + 1);

		if (H.h_end == H.h_st)
		{
			free(H.cmd[H.h_end]);
			H.h_st = (H.h_end + 1) % (HISTORY_SIZE + 1);
		}

		H.cmd[H.h_end] = strdup("");

		history_save();
	}
	H.curr_index = H.h_end; // bring the history pointer to empty prompt
}

void history_save() // saves the "history.txt" (to be called just before the shell is closed)
{
	FILE *fptr = fopen("history.txt", "w");
	if (fptr == NULL)
	{
		printf("Error in opening history\n");
		exit(EXIT_FAILURE);
	}

	// write back the contents of history buffer
	for (int i = H.h_st; i != H.h_end; i = (i + 1) % (HISTORY_SIZE + 1))
	{
		fprintf(fptr, "%s\n", H.cmd[i]);
	}
	fclose(fptr);
}

void history_print() // prints the history from oldest to latest
{
	int cnt = 1;
	// H.cmd[H.h_end] is always empty string, the empty prompt, history is upto just before H.h_end
	for (int i = H.h_st; i != H.h_end; i = (i + 1) % (HISTORY_SIZE + 1))
	{
		printf("%5d : %s\n", cnt, H.cmd[i]);
		cnt++;
	}
}

int up_key_handler(int count, int key) // handles up arrow key press to navigate in the history
{
	if (H.curr_index != H.h_st)
		H.curr_index = (H.curr_index + HISTORY_SIZE) %
					   (HISTORY_SIZE + 1); // move H.curr_index up in history

	rl_replace_line(H.cmd[H.curr_index], 0);
	rl_redisplay();
	rl_point = rl_end; // have the cursor at the end of the command displayed
	return 0;
}

int down_key_handler(int count, int key) // handles down arrow key press to navigate in the history
{
	if (H.curr_index != H.h_end)
		H.curr_index = (H.curr_index + 1) %
					   (HISTORY_SIZE + 1); // move H.curr_index down in history

	rl_replace_line(H.cmd[H.curr_index], 0);
	rl_redisplay();
	rl_point = rl_end; // have the cursor at the end of the command displayed
	return 0;
}
