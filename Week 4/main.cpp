#include <bits/stdc++.h>

#include "Action.hpp"
#include "Graph.hpp"
#include "Node.hpp"
using namespace std;

constexpr size_t SIZE{37'700};
vector<Node> nodes(SIZE);

void readGraph(
	Graph<Node> &graph,
	const string &filename)
{
	ifstream file{filename};
	if (not file.is_open())
	{
		cerr << "Failed to open file" << endl;
		exit(EXIT_FAILURE);
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

	// Set the neighbors
	for (size_t i = 0; i < SIZE; ++i)
	{
		nodes[i].setNeighbors(graph[i]); // Very important to do this before the next step
	}

	size_t edges{0};

	for (size_t i = 0; i < SIZE; ++i)
	{
		printf("Node %6zu has %6zu neighbors and capacity %6zu\n",
			   i,
			   graph[i].size(),
			   graph[i].capacity());
		edges += graph[i].size();
	}

	printf("Total edges: %zu Average degree: %lf\n", edges / 2, (double)edges / 2 / SIZE);
}
