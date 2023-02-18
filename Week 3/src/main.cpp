#include <bits/stdc++.h>
#include <sys/wait.h>

#include "Graph.hpp"
using namespace std;

int main()
{
	// Create shared memory
	key_t shmkey = ftok("input/facebook_combined.txt", 1);

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

	int u{}, v{};
	while (file >> u >> v)
	{
		graph(u, v) = true;
	}

	puts("Done reading file. File saved in shared memory! 🥳🥳");

	file.close();

	// Child for now just creates the dual graph (inverts the edges)

	pid_t pid = fork();

	if (pid == 0)
	{
		// Child process
		execl("./child",
			  "child",
			  to_string(shmkey).c_str(), // Convert key_t to string
			  "1",						 // Number of child process
			  nullptr);
	}
	else if (pid > 0)
	{
		// Parent process
		wait(nullptr);

		int cnt = 0;
		for (size_t i = 0; i < VEC_LEN; i++)
		{
			for (size_t j = 0; j < VEC_LEN; j++)
			{
				if (graph(i, j))
				{
					cnt++;
				}
			}
		}
		cout << "Parent Process: " << cnt << endl;
	}
	else
	{
		cout << "Error forking" << endl;
		return EXIT_FAILURE;
	}
}
