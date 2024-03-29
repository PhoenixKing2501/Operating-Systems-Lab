// #include <stdio.h>

// int main(int argc, char *argv[]) {
//     int pid;
//     int result;

//     result = sscanf(argv[1], "%d", &pid);
//     if (result == 1) {
//         printf("Input is a valid number\n");
//     } else {
//         printf("Input is not a valid number\n");
//     }
//     return 0;
// }

// #include <ncurses.h>

// int main()
// {	int ch;

// 	initscr();			/* Start curses mode 		*/
// 	raw();				/* Line buffering disabled	*/
// 	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
// 	noecho();			/* Don't echo() while we do getch */

//     	printw("Type any character to see it in bold\n");
// 	ch = getch();			/* If raw() hadn't been called
// 					 * we have to press enter before it
// 					 * gets to the program 		*/
// 	if(ch == KEY_F(1))		/* Without keypad enabled this will */
// 		printw("F1 Key pressed");/*  not get to us either	*/
// 					/* Without noecho() some ugly escape
// 					 * charachters might have been printed
// 					 * on screen			*/
// 	else
// 	{	printw("The pressed key is ");
// 		attron(A_BOLD);
// 		printw("%c", ch);
// 		attroff(A_BOLD);
// 	}
// 	refresh();			/* Print it on to the real screen */
//     	getch();			/* Wait for user input */
// 	endwin();			/* End curses mode		  */

// 	return 0;
// }
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

int color_mode = 1;

void toggle_color() {
  color_mode = !color_mode;
}

int main() {
  char *cmd;

  while ((cmd = readline("$ ")) != NULL) {
    if (strcmp(cmd, "ls") == 0) {
      DIR *d;
      struct dirent *dir;
      d = opendir(".");
      if (d) {
        while ((dir = readdir(d)) != NULL) {
          if (color_mode) {
            if (dir->d_type == DT_DIR) {
              printf("\033[34m%s\033[0m\n", dir->d_name);
            } else if (strstr(dir->d_name, ".c")) {
              printf("\033[32m%s\033[0m\n", dir->d_name);
            } else if (strstr(dir->d_name, ".txt")) {
              printf("\033[31m%s\033[0m\n", dir->d_name);
            } else {
              printf("%s\n", dir->d_name);
            }
          } else {
            printf("%s\n", dir->d_name);
          }
        }
        closedir(d);
      }
    } else if (strcmp(cmd, "toggle") == 0) {
      toggle_color();
    }

    add_history(cmd);
    free(cmd);
  }

  return 0;
}
