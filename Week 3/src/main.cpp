#include <bits/stdc++.h>
#include <sys/wait.h>
#include <errno.h>

#include "Graph.hpp"
using namespace std;

int main()
{
	// Create shared memory
	auto shmkey = ftok("/home/swarup/Desktop/SEM_6/OS_LAB/Week 3/input/facebook_combined.txt", 1);

	if (shmkey == -1)
	{
		fprintf(stderr, "ftok failed: %s\n", strerror(errno));
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

	puts("Done reading file. File saved in shared memory! 🥳🥳");
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
			// Consumer Process
			execl("./consumer",
				  "./consumer",
				  to_string(shmkey).c_str(),
				  to_string(i).c_str(),
				  nullptr);
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

	//wait on all processes
	for (int32_t i = 0; i < 11; i++)
	{
		wait(nullptr);
	}

	graph.remove_shm();
}
