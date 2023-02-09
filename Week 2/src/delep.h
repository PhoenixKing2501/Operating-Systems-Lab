#pragma once

#ifndef _DELEP_H_
#define _DELEP_H_

#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_PATH_LENGTH (1 << 10)
#define MAX_FILENAME_LEN (1 << 10)
#define MAX_PROCS_TO_KILL (1 << 10)

int delep_list_files(char *arg, int delep_pipefd);

#endif // _DELEP_H_
