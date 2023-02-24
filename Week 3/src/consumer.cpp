#include <bits/stdc++.h>

#include "Graph.hpp"
using namespace std;

int32_t num;

void djikstra(size_t src, Graph<bool> &graph)
{
    size_t n = graph.getSize();
    vector<size_t> dist(n, INT_MAX);
    vector<bool> visited(n, false);
    vector<size_t> last_visitied(n, -1);
    dist[src] = 0;
    last_visitied[src] = 0;
    for (size_t i = 0; i < n; i++)
    {
        size_t u = n;
        for (size_t j = 0; j < n; j++)
        {
            if (!visited[j] && (u == n || dist[j] < dist[u]))
            {
                u = j;
            }
        }
        visited[u] = true;
        for (size_t v = 0; v < n; v++)
        {
            if (graph(u, v) && dist[u] + 1 < dist[v])
            {
                dist[v] = dist[u] + 1;
                last_visitied[v] = u;
            }
        }
    }
    // get shortest path sequence of nodes and write to file
    ofstream file;
    file.open("consumer" + to_string(num) + ".txt", ios::app);
    for (size_t i = 0; i < n; i++)
    {
        if (i == src)
        {
            continue;
        }
        file << src << " " << i << " " << dist[i] << " ";
        // get path in a vector and reverse it
        vector<size_t> path;
        size_t temp = i;
        while (temp != src)
        {
            path.push_back(temp);
            temp = last_visitied[temp];
        }
        reverse(path.begin(), path.end());
        for (auto x : path)
        {
            file << x << " ";
        }
        file << endl;
    }
    file.close();
}
int32_t main(int32_t argc, char const *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: ./consumer <shmkey>\n";
        cout << "Usage: ./consumer <number>\n";
        exit(EXIT_FAILURE);
    }

    Graph<bool> graph(stoi(argv[1]));

    // count the number of unique nodes in graph
    while (true)
    {
        this_thread::sleep_for(chrono::seconds(5));
        size_t count = 0;
        for (size_t i = 0; i < graph.getSize(); i++)
        {
            for (size_t j = 0; j < graph.getSize(); j++)
            {
                if (graph(i, j) == true)
                {
                    count++;
                }
            }
        }

        cout << "Number of unique nodes: " << count << endl;

        num = stoi(argv[2]);

        size_t st = (num * count) / 10 + 1;
        size_t en = ((num + 1) * count) / 10;

        cout << "Number of nodes mapped to " << num << " consumer: " << en - st + 1 << endl;

        // now compute djisktra for each of this nodes to all other nodes and append to a file consumer<num>.txt

        for (size_t i = st; i <= en; i++)
        {
            // compute djisktra for node i
            djikstra(i, graph);
        }
    }
    // two arguments first is key, second the number (which 1/10th of nodes mapped to it)
}
