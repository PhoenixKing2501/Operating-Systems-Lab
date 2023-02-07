#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>

#define HISTORY_SIZE 1001

typedef struct
{
    char *cmd[HISTORY_SIZE];
    int curr_index;
    int h_st;
    int h_end;
} history_t;

history_t H;

history_t initHistory()
{
    history_t H;
    H.h_st = 0;
    H.h_end = 0;
    H.cmd[H.h_st] = "";
    H.curr_index = H.h_st;

    return H;
}

void history_add(char *cmd)
{
    H.cmd[H.h_st] = strdup(cmd);
    H.h_st = (H.h_st + 1) % HISTORY_SIZE;
    H.curr_index = H.h_st;
    if (H.cmd[H.h_st])
        free(H.cmd[H.h_st]);
    H.cmd[H.h_st] = "";
    if (H.h_st == H.h_end)
        H.h_end = H.h_st + 1;
}

void history_print()
{
    int cnt = 0;
    for (int i = H.h_st; i != H.h_end; i = (i + 1) % HISTORY_SIZE) // cmc[h_st] is always empty string, the empty prompt
    {
        printf("%d: %s\n", cnt, H.cmd[i]);
    }
    printf("%d: %s\n", cnt, H.cmd[H.h_end]);
}

int up_key_handler(int count, int key)
{
    if (H.curr_index != H.h_end)
        H.curr_index = (H.curr_index - 1 + HISTORY_SIZE) % HISTORY_SIZE;

    rl_replace_line(H.cmd[H.curr_index], 0);
    rl_redisplay();
    return 0;
}

int down_key_handler(int count, int key)
{
    if (H.curr_index != H.h_st)
        H.curr_index = (H.curr_index + 1) % HISTORY_SIZE;

    rl_replace_line(H.cmd[H.curr_index], 0);
    rl_redisplay();
    return 0;
}

int main()
{
    H = initHistory(); // cmd[h_st] is always empty string for empty prompt

    rl_bind_keyseq("\\e[A", up_key_handler);
    rl_bind_keyseq("\\e[B", down_key_handler);

    char *line;
    while (true)
    {
        line = readline("> ");
        printf("line: %s (%lu)\n", line, strlen(line));
        if (strlen(line) > 0)
        {
            history_add(line);
        }
        free(line);
    }
}