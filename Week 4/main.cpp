#include <bits/stdc++.h>

#include "Action.hpp"
#include "Graph.hpp"
#include "Node.hpp"

using namespace std;

constexpr size_t SIZE{37'700};
vector<Node> nodes(SIZE);
queue<Action> shared_queue{};

/*declare a mutex and a condition variable*/
pthread_mutex_t shared_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t shared_queue_cond = PTHREAD_COND_INITIALIZER;

extern void *userSimulatorRunner(void *);
extern void *pushUpdateRunner(void *);
extern void *readPostRunner(void *);

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
		nodes[i].setNeighbors(&graph[i]); // Very important to do this before the next step
	}

	// size_t edges{0};

	// for (size_t i = 0; i < SIZE; ++i)
	// {
	// 	printf("Node %6zu has %6zu neighbors and capacity %6zu\n",
	// 		   i,
	// 		   nodes[i].neighbors->size(),
	// 		   nodes[i].neighbors->capacity());
	// 	edges += graph[i].size();
	// }

	// printf("Total edges: %zu Average degree: %lf\n", edges / 2, (double)edges / 2 / SIZE);

	// Create the threads
	// create one user thread
	// 25 pushUpdate threads
	// 10 readPost threads

	array<pthread_t, 36> threads{};

	pthread_create(&threads[0], NULL, userSimulatorRunner, &graph);
	fputs("User thread created\n", stderr);
	for (int i = 1; i < 26; i++)
	{
		pthread_create(&threads[i], NULL, pushUpdateRunner, &graph);
	}
	fputs("PushUpdate threads created\n", stderr);

	for (int i = 26; i < 36; i++)
	{
		int *num = new int{i - 26};
		pthread_create(&threads[i], NULL, readPostRunner, num);
		fprintf(stderr, "ReadPost thread %d created\n", i);
	}
	fputs("ReadPost threads created\n", stderr);

	// this_thread::sleep_for(chrono::seconds(120));

	// Join the threads
	for (int i = 0; i < 36; i++)
	{
		pthread_join(threads[i], NULL);
	}
	// pthread_exit(NULL);
}
