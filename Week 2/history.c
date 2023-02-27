#include <ncurses.h>
#include <string.h>

#define MAX_HISTORY 1000
#define MAX_LINE 80

char history[MAX_HISTORY][MAX_LINE];
int history_index = 0;
int current_history = 0;

int main(void)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    int row, col;

    while (1)
    {
        getyx(stdscr, row, col);
        move(row, 0);
        clrtoeol();
        printw(">");

        char input[MAX_LINE];
        int ch, i = 0;

        while ((ch = getch()) != '\n')
        {
            const char *name = keyname(ch);
            if (ch == KEY_UP)
            {
                if (current_history == 0)
                {
                    continue;
                }
                current_history--;
                strcpy(input, history[current_history % MAX_HISTORY]);
                i = strlen(input);
                getyx(stdscr, row, col);
                move(row, 1);
                clrtoeol();
                printw("%s", input);
            }
            else if (ch == KEY_DOWN)
            {
                if (current_history == history_index)
                {
                    continue;
                }
                current_history++;
                strcpy(input, history[current_history % MAX_HISTORY]);
                i = strlen(input);
                getyx(stdscr, row, col);
                move(row, 1);
                clrtoeol();
                printw("%s", input);
            }
            else
            {
                input[i++] = ch; // keep input read uptil now
                addch(ch);       // this also keeps printing to terminal
            }
        }
        input[i] = '\0';
        // if (flag == 1)
        // {
        //     printw("%s\n", input);
        // }
        if (strlen(input) > 0)
        {
            // don't add to history if nothing is typed
            strcpy(history[history_index % MAX_HISTORY], input);
            history_index++;
        }
        current_history = history_index; // make current history pointer to the last history
        printw("\n");
        refresh();
    }

    endwin();
    return 0;
}
