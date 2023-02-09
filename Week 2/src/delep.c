#include "delep.h"
int str_is_numeric(char *str)
{
	int is_numeric = 1;
	while (*str != '\0')
	{
		if (*str < '0' || *str > '9')
		{
			is_numeric = 0;
			break;
		}
		str++;
	}
	return is_numeric;
}

int insert_unique(pid_t to_kill_arr[], pid_t to_add, int *unique_procs_cnt)
{
	int i;
	for (i = 0; i < MAX_PROCS_TO_KILL && to_kill_arr[i] != 0; ++i)
	{
		if (abs(to_add) == abs(to_kill_arr[i]))
			return 1;
	}
	if (i < MAX_PROCS_TO_KILL)
	{
		to_kill_arr[i] = to_add;
		(*unique_procs_cnt)++;
		return 0;
	}
	return -1;
}

void parse_file_details(
	char *line,
	unsigned int *major_device,
	unsigned int *minor_device,
	ino_t *inode,
	pid_t *locking_proc)
{
	char *next_tok = strtok(line, " ");
	int tok_idx = -1;
	while (next_tok != NULL)
	{
		tok_idx++;
		if (tok_idx == 4)
		{
			*locking_proc = atoi(next_tok);
		}
		else if (tok_idx == 5)
		{
			break;
		}
		next_tok = strtok(NULL, " ");
	}
	// file id obtained in next_tok, spit next_tok using : delimiter
	char *file_id = next_tok;
	tok_idx = -1;

	sscanf(file_id, "%u:%u:%lu", major_device, minor_device, inode);
}

int delep_list_files(char *arg, int delep_pipefd)
{
	struct stat st;
	char filename[MAX_PATH_LENGTH];
	realpath(arg, filename);
	unsigned int target_major_device, target_minor_device;
	int unique_procs_to_kill = 0;
	ino_t target_inode;
	pid_t procs_to_kill[MAX_PROCS_TO_KILL] = {0};

	if (stat(filename, &st) == 0)
	{
		target_major_device = major(st.st_dev);
		target_minor_device = minor(st.st_dev);
		target_inode = st.st_ino;
	}
	else
	{
		perror("stat failed\n");
		return -1;
	}

	// printf("List of process IDs holding lock over concerned file:\n");
	size_t len = 0;
	size_t read_len;
	char *line = NULL;
	FILE *lock_info = fopen("/proc/locks", "r");
	if (lock_info == NULL)
	{
		puts("cannot open /proc/locks");
		return -1;
	}
	while ((read_len = getline(&line, &len, lock_info)) != -1)
	{
		pid_t locking_proc;
		unsigned int major_device, minor_device;
		ino_t inode;
		parse_file_details(line, &major_device, &minor_device, &inode, &locking_proc);

		if (major_device == target_major_device &&
			minor_device == target_minor_device &&
			inode == target_inode)
		{
			// printf("+ %d\n",locking_proc);
			insert_unique(procs_to_kill, -locking_proc, &unique_procs_to_kill);
		}
	}
	fclose(lock_info);

	char proc_fd_dir_name[MAX_PATH_LENGTH];
	char proc_fd_link[MAX_PATH_LENGTH];
	char proc_fd_target[MAX_FILENAME_LEN];

	char *proc_path = "/proc";
	DIR *proc_dir = opendir(proc_path);
	struct dirent *proc_ent;

	// printf("List of process IDs having concerned file open:\n");
	while ((proc_ent = readdir(proc_dir)) != NULL)
	{
		if (str_is_numeric(proc_ent->d_name))
		{
			sprintf(proc_fd_dir_name, "/proc/%d/fd", atoi(proc_ent->d_name));

			DIR *proc_fd_dir = opendir(proc_fd_dir_name);
			if (!proc_fd_dir)
			{
				continue;
			}
			struct dirent *proc_fd_dir_ent;
			while ((proc_fd_dir_ent = readdir(proc_fd_dir)) != NULL)
			{
				if (!str_is_numeric(proc_fd_dir_ent->d_name))
					continue;
				sprintf(proc_fd_link, "/proc/%d/fd/%s", atoi(proc_ent->d_name), proc_fd_dir_ent->d_name);

				ssize_t read_len = readlink(proc_fd_link, proc_fd_target, MAX_FILENAME_LEN - 1);
				if (read_len < 0)
				{
					perror("readlink");
					return -1;
				}

				proc_fd_target[read_len] = '\0';
				if (!strcmp(proc_fd_target, filename))
				{
					// printf("+ %s\n",proc_ent->d_name);
					insert_unique(procs_to_kill, atoi(proc_ent->d_name), &unique_procs_to_kill);
				}
			}

			closedir(proc_fd_dir);
		}
	}
	closedir(proc_dir);

	printf("Child: %d\n", unique_procs_to_kill);
	if (write(delep_pipefd, &unique_procs_to_kill, sizeof(unique_procs_to_kill)) < 0)
	{
		return -1;
	}
	if (write(delep_pipefd, procs_to_kill, sizeof(pid_t) * unique_procs_to_kill) < 0)
	{
		return -1;
	}

	return 0;
}
