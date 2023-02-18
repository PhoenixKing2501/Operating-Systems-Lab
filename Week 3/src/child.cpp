#include <bits/stdc++.h>

#include "Graph.hpp"
using namespace std;

int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		cout << "Usage: ./child <shmkey> <number>" << endl;
		return EXIT_FAILURE;
	}

	Graph<bool> graph(stoi(argv[1]));
	cout << "Number: " << argv[2] << endl;

	int cnt{};
	for (size_t i{}; i < VEC_LEN; ++i)
	{
		for (size_t j{}; j < VEC_LEN; ++j)
		{
			if (graph(i, j))
			{
				++cnt;
			}
			graph(i, j) = not graph(i, j);
		}
	}

	cout << "Child Process: " << cnt << endl;
}
