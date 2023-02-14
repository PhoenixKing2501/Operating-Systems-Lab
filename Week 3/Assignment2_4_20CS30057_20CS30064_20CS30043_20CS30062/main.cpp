#include <bits/stdc++.h>

#include <sys/wait.h>

#include "My_Allocator.hpp"
using namespace std;

int main()
{
	vector<vector<int32_t, My_Allocator<int32_t>>,
		   My_Allocator<vector<int32_t, My_Allocator<int32_t>>>>
		graph(VEC_LEN, vector<int32_t, My_Allocator<int32_t>>(VEC_LEN));

	puts("Reading file...");

	fstream file("input/facebook_combined.txt", ios::in);
	if (not file.is_open())
	{
		cout << "Error opening file" << endl;
		return EXIT_FAILURE;
	}

	int u, v;
	while (file >> u >> v)
	{
		graph[u][v] = true;
	}

	puts("Done reading file. File saved in shared memory! 🥳🥳");

	file.close();

	// Child for now just creates the dual graph (inverts the edges)

	pid_t pid = fork();

	if (pid == 0)
	{
		// Child process
		int cnt = 0;
		for (size_t i = 0; i < VEC_LEN; i++)
		{
			for (size_t j = 0; j < VEC_LEN; j++)
			{
				if (graph[i][j])
				{
					cnt++;
				}
				graph[i][j] = 1 - graph[i][j];
			}
		}
		cout << "Child process: " << cnt << endl;
		exit(EXIT_SUCCESS);
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
				if (graph[i][j])
				{
					cnt++;
				}
			}
		}
		cout << "Parent process: " << cnt << endl;
	}
	else
	{
		cout << "Error forking" << endl;
		return EXIT_FAILURE;
	}
}
