#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LEN 1024
#define STAT_FILE "/proc/%d/stat"

void get_process_info(pid_t pid) {
  char file_path[MAX_LEN];
  sprintf(file_path, STAT_FILE, pid);

  int fd = open(file_path, O_RDONLY);
  if (fd == -1) {
    printf("Failed to open file for pid %d\n", pid);
    return;
  }

  char buffer[MAX_LEN];
  ssize_t bytes_read = read(fd, buffer, MAX_LEN);
  if (bytes_read == -1) {
    printf("Failed to read data for pid %d\n", pid);
    return;
  }

  close(fd);

  buffer[bytes_read] = '\0';

  char *token = strtok(buffer, " ");
  int count = 0;
  char *process_name = NULL;
  char *process_state = NULL;

  while (token) {
    switch (count) {
      case 1:
        process_name = token;
        break;
      case 2:
        process_state = token;
        break;
    }
    token = strtok(NULL, " ");
    count++;
  }

  printf("pid: %d, name: %s, state: %s\n", pid, process_name, process_state);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <pid1> <pid2> ...\n", argv[0]);
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    pid_t pid = atoi(argv[i]);
    get_process_info(pid);
  }

  return 0;
}
