#include <bits/stdc++.h>

#include "Graph.hpp"
#include "Node.hpp"
using namespace std;

constexpr size_t SIZE{37'700};

void readGraph(
	Graph<Node> &graph,
	const string &filename)
{
	ifstream file{filename};
	if (!file.is_open())
	{
		cerr << "Failed to open file" << endl;
		return;
	}

	string line{};
	getline(file, line); // Skip the first line

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
}

int main()
{
	// Create the graph
	Graph<Node> graph{SIZE};

	// Read the graph
	readGraph(graph, "input/musae_git_edges.csv");

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
