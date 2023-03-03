#include <bits/stdc++.h>

#include "Graph.hpp"
#include "Node.hpp"
using namespace std;

constexpr size_t SIZE{37'700};

int main()
{
	ifstream file{"input/musae_git_edges.csv"};
	if (!file.is_open())
	{
		cerr << "Failed to open file" << endl;
		return EXIT_FAILURE;
	}

	string line{};
	getline(file, line); // Skip the first line

	// Create the graph
	Graph<Node> graph{SIZE};

	// Read the file
	while (getline(file, line))
	{
		// Split the line
		istringstream iss{line};
		string token{};
		vector<string> tokens{};
		while (getline(iss, token, ','))
		{
			tokens.push_back(token);
		}

		// Add the edge
		graph.addEdge(stoul(tokens[0]), stoul(tokens[1]));
	}

	file.close();

	// Shrink the graph
	graph.shrinkToFit();

	for (size_t i = 0; i < SIZE; ++i)
	{
		printf("Node %6zu has %6zu neighbors and capacity %6zu \n",
			   i,
			   graph[i].size(),
			   graph[i].capacity());
		cout << boolalpha << (graph[i].size() == graph[i].capacity()) << endl;
	}
}
