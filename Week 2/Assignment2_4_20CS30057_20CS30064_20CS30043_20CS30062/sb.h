#pragma once

#ifndef _SB_H_
#define _SB_H_

#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXBUF (BUFSIZ * 2)
#define MAX_PROCESS_NAME_SIZE 50 // max process name size

int squashbug(char *argv[]);

#endif // _SB_H_
