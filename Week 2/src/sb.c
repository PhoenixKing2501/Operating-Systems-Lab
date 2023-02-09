#include "sb.h"

int detectbug = 0; // flag to check if user entered --suggest flag
double maxval = 0; // max value of children/(utime+stime+1)
int maxpid = -1;

/*function to count numbers in a line separated by space*/
int count_numbers(char *line)
{
	int count = 0;
	char *token = strtok(line, " ");
	while (token != NULL)
	{
		++count;
		token = strtok(NULL, " ");
	}
	return count;
}

/*function to get parent process id and
	number of children as well as active time
	of a process */
int get_parent(pid_t pid)
{
	int ppid;
	char buf[MAXBUF];
	char procname[32]; // /proc/<pid>/status
	FILE *fp;
	char name[MAX_PROCESS_NAME_SIZE]; // process name

	snprintf(procname, sizeof(procname), "/proc/%u/status", pid);
	fp = fopen(procname, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Process %d not found\n", pid);
		exit(-1);
	}
	while (fgets(buf, sizeof(buf), fp) != NULL) // read line by line and find line containing name and ppid
	{
		if (strstr(buf, "Name:") == buf)
		{
			sscanf(buf + 6, "%s", name);
		}
		if (strstr(buf, "PPid:") == buf)
		{
			sscanf(buf + 6, "%d", &ppid);
			break;
		}
	}

	if (detectbug) // flag to check if user entered --suggest flag
	{
		int child_count = 0; // count number of children
		char dir_name[64];

		// Check number of child processes
		sprintf(dir_name, "/proc/%d/task/%d/children", pid, pid);
		FILE *ch = fopen(dir_name, "r");
		char line[1000];
		while (fgets(line, sizeof(line), ch) != NULL)
		{
			child_count += count_numbers(line);
		}
		fclose(ch);
		// printf("Number of children of process %d is %d\n", pid, child_count);

		// Calculate utime and stime
		int non_sleep_time;
		int utime, stime, cutime, cstime;
		sprintf(dir_name, "/proc/%d/stat", pid);
		FILE *fp = fopen(dir_name, "r");
		if (fp == NULL)
		{
			fprintf(stderr, "Process %d not found\n", pid);
			exit(1);
		}
		if (fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d "
					   "%*u %*u %*u %*u %*u %d %d %d %d",
				   &utime, &stime, &cutime, &cstime) == 4)
		{
			non_sleep_time = utime + stime;
			fclose(fp);

			double val = (child_count + 1) /
						 (1 + log(non_sleep_time + 1)); // +1 to consider case when utime+stime = 0
			if (val > maxval)
			{
				maxval = val;
				maxpid = pid;
			}
		}
	}
	printf("Name: %s, PID: %d, PPID: %d\n", name, pid, ppid);
	return ppid;
}
/*function to print ancestors of a process*/
void print_ancestors(pid_t pid)
{
	pid_t ppid;
	while (pid != 0)
	{

		ppid = get_parent(pid);
		if (ppid == -1)
			return;
		pid = ppid;
	}
}

/*function to parse the arguments*/
int squashbug(char *argv[])
{
	if (argv[1] == NULL)
	{
		fprintf(stderr, "shell: expected argument to \"sb\"\n");
		return 1;
	}

	detectbug = 0;
	maxval = 0;
	maxpid = -1;

	pid_t pid;
	int ret = 0;
	if (argv[2] == NULL)
	{
		ret = sscanf(argv[1], "%d", &pid);
	}
	else
	{
		if (strcmp(argv[1], "--suggest") == 0)
		{
			ret = sscanf(argv[2], "%d", &pid);

			detectbug = 1;
		}
		else if (strcmp(argv[2], "--suggest") == 0)
		{
			ret = sscanf(argv[1], "%d", &pid);

			detectbug = 1;
		}
		else
		{
			if (ret == 0)
			{
				fprintf(stderr, "The process id is not in correct format\n");
			}
			fprintf(stderr, "Incorrect flag specified with bash command \"sb\"\n");
			return 1;
		}
	}
	if (ret == 0)
	{
		fprintf(stderr, "The process id is not in correct format\n");
	}
	print_ancestors(pid); // print ancestors of process

	if (detectbug)
	{
		printf("\nSuspected process to kill: %d\n", maxpid);
	}

	return 0;
}
