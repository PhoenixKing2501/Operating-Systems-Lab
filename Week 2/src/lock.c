#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
	char *filepath = "sample.txt";
	pid_t pid;
	int wstatus;
	if ((pid = fork()) == 0)
	{
		int fd = open(filepath, O_WRONLY | O_CREAT);
		if (fd == -1)
		{
			perror("open");
			return 1;
		}

		if (flock(fd, LOCK_EX) == -1)
		{ // LOCK_EX for exclusive (write) lock
			perror("flock");
			exit(EXIT_FAILURE);
		}

		while (1)
		{
			write(fd, "", 0);
		}

		close(fd);
	}

	do
	{
		waitpid(pid, &wstatus, WUNTRACED);
	} while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

	return 0;
}
