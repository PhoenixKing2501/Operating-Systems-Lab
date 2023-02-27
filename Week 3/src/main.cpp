#include <bits/stdc++.h>
#include <sys/wait.h>
#include <errno.h>

#include "Graph.hpp"
using namespace std;

int main(int argc, char const *argv[])
{
	bool opt_flag = false;
	// check if -optimize flag specified by user
	if (argc == 2)
	{
		if (strcmp(argv[1], "-optimize") == 0)
		{
			opt_flag = true;
		}
		else
		{
			cout << "Invalid option\n";
			exit(EXIT_FAILURE);
		}
	}
	// Create shared memory
	auto shmkey = ftok("input/facebook_combined.txt", 1);

	if (shmkey == -1)
	{
		perror("ftok");
		return EXIT_FAILURE;
	}

	Graph<bool> graph(shmkey);
	graph.init(false);

	puts("Reading file...");

	fstream file("input/facebook_combined.txt", ios::in);
	if (not file.is_open())
	{
		cout << "Error opening file" << endl;
		return EXIT_FAILURE;
	}

	int32_t u{}, v{};
	while (file >> u >> v)
	{
		graph(u, v) = true;
		graph(v, u) = true;
	}

	puts("Done reading file. File saved in shared memory!");
	file.close();

	auto pid = fork();
	if (pid == 0)
	{
		// Producer Process
		execl("./producer",
			  "./producer",
			  to_string(shmkey).c_str(),
			  nullptr);
	}
	else if (pid == -1)
	{
		cout << "Error forking" << endl;
		return EXIT_FAILURE;
	}

	// start 10 consumer processes
	for (int32_t i = 0; i < 10; i++)
	{
		auto pid = fork();
		if (pid == 0)
		{
			if (opt_flag)
			{
				// Optimized Consumer Process
				execl("./consumer",
					  "./consumer",
					  to_string(shmkey).c_str(),
					  to_string(i).c_str(),
					  "-optimize",
					  nullptr);
			}
			else
			{
				// Consumer Process
				execl("./consumer",
					  "./consumer",
					  to_string(shmkey).c_str(),
					  to_string(i).c_str(),
					  nullptr);
			}
		}
		else if (pid > 0)
		{
			continue;
		}
		else
		{
			cout << "Error forking" << endl;
			return EXIT_FAILURE;
		}
	}

	// wait on all processes
	for (int32_t i = 0; i < 11; i++)
	{
		wait(nullptr);
	}

	graph.remove_shm();
}
