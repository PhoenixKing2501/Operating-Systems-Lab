#pragma once

#ifndef _HISTORY_H_
#define _HISTORY_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>

#define HISTORY_SIZE 1000

typedef struct
{
	char *cmd[HISTORY_SIZE + 1];
	int curr_index;
	int h_st;
	int h_end;
} history_t;

void init_history();
void history_add(char *cmd);
void history_print();
int up_key_handler(int count, int key);
int down_key_handler(int count, int key);

extern history_t H;

#endif // _HISTORY_H_
