#include <bits/stdc++.h>

#include "Graph.hpp"
using namespace std;

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		cout << "Usage: ./producer <shmkey>" << endl;
		return EXIT_FAILURE;
	}

	Graph<bool> graph(stoi(argv[1]));

	// mt19937_64 gen{random_device{}()};
	mt19937_64 gen{120};
	uniform_int_distribution<int32_t> dist1{10, 30};
	uniform_int_distribution<int32_t> dist2{1, 20};

	uniform_real_distribution<double> dist3{0.0, 1.0};

	while (true)
	{
		this_thread::sleep_for(chrono::seconds(25));

		int32_t m = dist1(gen);
		size_t old_size = graph.getSize();
		size_t new_size = graph.getSize() + m;

		if (new_size > VEC_LEN)
		{
			cout << "\nERROR: Graph is full"
					"\nNo more nodes can be added\n\n";
			continue;
		}

		graph.setSize(new_size);
		for (size_t i = old_size; i < new_size; ++i)
		{
			vector<pair<double, size_t>> v{};
			for (size_t j = 0; j < old_size; ++j)
			{
				v.push_back({dist3(gen) * graph.getDegree(j), j});
			}
			priority_queue<pair<double, size_t>> pq{v.begin(), v.end()};

			int32_t n = dist2(gen);

			for (int j = 0; j < n; ++j)
			{
				if (pq.empty())
				{
					break;
				}

				auto [_, u] = pq.top();
				pq.pop();
				graph(i, u) = true;
				graph(u, i) = true;
			}
		}

		cout << "New size from Producer: " << graph.getSize() << endl;

		// print the adjacency matrix to a file
		//^Uncomment this to print the adjacency matrix to a file
		// ofstream ofFile("adjacency_matrix.txt");
		// ofFile << graph.getSize() << "\n";
		// for (size_t i = 0; i < graph.getSize(); ++i)
		// {
		// 	for (size_t j = 0; j < graph.getSize(); ++j)
		// 	{
		// 		ofFile << graph(i, j) << " ";
		// 	}
		// 	ofFile << "\n";
		// }
		// ofFile.close();
	}
}
