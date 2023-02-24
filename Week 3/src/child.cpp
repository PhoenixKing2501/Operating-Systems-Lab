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

	const Graph<bool> graph(stoi(argv[1]));
	cout << "Number: " << argv[2] << endl;

	// uint64_t cnt{};
	// for (size_t i{}; i < graph.getSize(); ++i)
	// {
	// 	for (size_t j{}; j < i; ++j)
	// 	{
	// 		if (graph(i, j))
	// 		{
	// 			++cnt;
	// 		}
	// 		graph(i, j) = not graph(i, j);
	// 		graph(j, i) = not graph(j, i);
	// 	}
	// }

	// cout << "Child Process: " << cnt << endl;
}
