#include "Common.hpp"

constexpr size_t SIZE{37'700};
vector<Node> nodes{};
Queue<Action, 25> shared_queue{};
Queue<size_t, 10> updates{};
FILE *fptr = fopen("sns.log", "w");

/*declare a mutex and a condition variable*/
// pthread_mutex_t shared_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t shared_queue_cond = PTHREAD_COND_INITIALIZER;

char *get_time(
	time_t unix_timestamp)
{
	// make a dynamic array of 80 characters using new
	static char time_buf[100]{};
	struct tm ts = *localtime(&unix_timestamp);
	strftime(time_buf, sizeof(time_buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	return time_buf;
}
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
	nodes.reserve(SIZE);
	for (size_t i = 0; i < SIZE; ++i)
	{
		sort(begin(graph[i]), end(graph[i])); // Sort the neighbors for faster search
		nodes.emplace_back(i, &graph[i]);	  // Create the node and set the neighbors
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

	pthread_create(&threads[0], nullptr, userSimulatorRunner, &graph);
	for (int i = 1; i < 26; i++)
	{
		int *num = new int{i - 1};
		pthread_create(&threads[i], nullptr, pushUpdateRunner, num);
	}

	for (int i = 26; i < 36; i++)
	{
		int *num = new int{i - 26};
		pthread_create(&threads[i], nullptr, readPostRunner, num);
	}

	for (;;)
	{
		this_thread::sleep_for(chrono::seconds(5));
		fflush(stdout);
		fflush(fptr);
	}

	// Join the threads
	// for (int i = 0; i < 36; i++)
	// {
	// 	pthread_join(threads[i], NULL);
	// }
	// pthread_exit(NULL);
}
