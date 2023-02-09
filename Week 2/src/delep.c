#include "delep.h"

/* tests if all characters of a string is numberic */
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

/* insert to_add to the array to_kill_arr only if not already present(set insert) */
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

/*
parses a line provided in the format as stored in /proc/locks and extracts the fields:
1. process_id
2. major_device, minor_device, inode (specifying file_id)
*/
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
		if (tok_idx == 4) /* process id holding the lock */
		{
			*locking_proc = atoi(next_tok);
		}
		else if (tok_idx == 5) /* id of the file that is locked in the format major-device:minor-device:inode */
		{
			break;
		}
		next_tok = strtok(NULL, " ");
	}
	// file id obtained in next_tok, spit next_tok using ':' as delimiter
	char *file_id = next_tok;
	tok_idx = -1;

	sscanf(file_id, "%u:%u:%lu", major_device, minor_device, inode);
}

/*
lists down the process ids which have either opened or locked the file (filename specified by arg) and
writes the pids to a pipe (to be read and displayed to user by parent process)
*/
int delep_list_files(char *arg, int delep_pipefd)
{
	struct stat st;
	char filename[MAX_PATH_LENGTH];
	realpath(arg, filename); /* store the absolute path of the file into filename */
	unsigned int target_major_device, target_minor_device;
	int unique_procs_to_kill = 0;
	ino_t target_inode;
	pid_t procs_to_kill[MAX_PROCS_TO_KILL] = {0}; /* initialize the to_kill array with all 0s*/

	if (stat(filename, &st) == 0) /* get the details of the target file into a stat structure */
	{
		/* store the major_device, minor_device and inode of the target file */
		target_major_device = major(st.st_dev);
		target_minor_device = minor(st.st_dev);
		target_inode = st.st_ino;
	}
	else
	{
		perror("stat failed\n");
		return -1;
	}

	/* read the /proc/locks file and find processes that have the target file open */
	size_t len = 0;
	size_t read_len;
	char *line = NULL;
	FILE *lock_info = fopen("/proc/locks", "r");
	if (lock_info == NULL)
	{
		puts("cannot open /proc/locks");
		return -1;
	}
	/* read /proc/locks line by line */
	while ((read_len = getline(&line, &len, lock_info)) != -1)
	{
		pid_t locking_proc;
		unsigned int major_device, minor_device;
		ino_t inode;
		parse_file_details(line, &major_device, &minor_device, &inode, &locking_proc);

		/* compare parsed major_device, minor_device, inode with targer values */
		if (major_device == target_major_device &&
			minor_device == target_minor_device &&
			inode == target_inode)
		{
			insert_unique(procs_to_kill, -locking_proc, &unique_procs_to_kill);
			/* pid of a locked file is negated and inserted into to_kill array
			(pids which have only kept the file open will be inserted as it is,
			the negative values will help parent process in distinguishing pids
			based on wheter they have locked the file) */
		}
	}
	fclose(lock_info);

	char proc_fd_dir_name[MAX_PATH_LENGTH];
	char proc_fd_link[MAX_PATH_LENGTH];
	char proc_fd_target[MAX_FILENAME_LEN];

	char *proc_path = "/proc";
	DIR *proc_dir = opendir(proc_path);
	struct dirent *proc_ent;

	/* traverse the /proc dir, all numeric filenames refer to processes */
	while ((proc_ent = readdir(proc_dir)) != NULL)
	{
		if (str_is_numeric(proc_ent->d_name))
		{
			sprintf(proc_fd_dir_name, "/proc/%d/fd", atoi(proc_ent->d_name));

			/* traverse the fd directory for file descriptors that the process has kept open */
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
				/* store the filename corresponding to the fd into proc_fd_target */

				if (read_len < 0)
				{
					perror("readlink");
					return -1;
				}

				proc_fd_target[read_len] = '\0';
				if (!strcmp(proc_fd_target, filename))
				{
					/* insert pid into to_kill array in case filename matched */
					insert_unique(procs_to_kill, atoi(proc_ent->d_name), &unique_procs_to_kill);
				}
			}

			closedir(proc_fd_dir);
		}
	}
	closedir(proc_dir);

	if (write(delep_pipefd, &unique_procs_to_kill, sizeof(unique_procs_to_kill)) < 0)
	{
		/* write the number of uniques pids found into the pipe */
		return -1;
	}
	if (write(delep_pipefd, procs_to_kill, sizeof(pid_t) * unique_procs_to_kill) < 0)
	{
		/* write the to_kill array into the pipe */
		return -1;
	}

	return 0;
}
