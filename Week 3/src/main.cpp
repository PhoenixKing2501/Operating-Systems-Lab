#include <bits/stdc++.h>
#include <sys/wait.h>

#include "Graph.hpp"
using namespace std;

int main()
{
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
	else if (pid > 0)
	{
		while (true)
		{
			this_thread::sleep_for(chrono::seconds(3));

			cout << "Parent Process: " << graph.getSize() << endl;
		}
	}
	else
	{
		cout << "Error forking" << endl;
		return EXIT_FAILURE;
	}

	graph.remove_shm();
}
