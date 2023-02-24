#include <bits/stdc++.h>

#include "Graph.hpp"
using namespace std;

int32_t num;
int32_t iteration = 0;
ofstream file;
size_t nodes;

void djikstra(size_t src, Graph<bool> &graph)
{
    // cout << "num: " << num << "src: " << src << "\n";
    // use priority queue instead of vector
    priority_queue<pair<double, size_t>, vector<pair<double, size_t>>, greater<pair<double, size_t>>> pq;
    vector<double> dist(nodes, numeric_limits<double>::max());
    vector<size_t> last_visitied(nodes, -1);
    dist[src] = 0;
    pq.push({0, src});
    while (!pq.empty())
    {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u])
            continue;
        for (size_t v = 0; v < nodes; v++)
        {
            if (graph(u, v))
            {
                if (dist[v] > dist[u] + 1)
                {
                    dist[v] = dist[u] + 1;
                    last_visitied[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
        // cout << "Still doing for node " << src << "\n";
    }

    // get shortest path sequence of nodes and write to file and gzip it

    for (size_t i = 0; i < nodes; i++)
    {
        if (i == src)
            continue;
        vector<size_t> path;
        for (size_t j = i; j != src; j = last_visitied[j])
        {
            path.push_back(j);
        }
        file << src << " " << i << ":" << dist[i] << ":";
        path.push_back(src);
        reverse(path.begin(), path.end());
        for (size_t j = 0; j < path.size(); j++)
        {
            file << path[j] << " ";
        }
        file << "\n";
    }
    // string command = "gzip " + filename;
    // system(command.c_str());
}
int32_t main(int32_t argc, char const *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: ./consumer <shmkey> <number>\n";
        exit(EXIT_FAILURE);
    }
    num = stoi(argv[2]);
    Graph<bool> graph(stoi(argv[1]));
    while (true)
    {
        string filename = "consumer" + to_string(num) + ".txt";
        // open file in append mode,create if not exists
        file.open(filename, ios::out | ios::app);
        this_thread::sleep_for(chrono::seconds(2));
        // nodes the number of unique nodes in graph
        iteration++;
        file << "Iteration: " << iteration << "\n";
        nodes = graph.getSize();

        cout << "Number of unique nodes: " << nodes << "\n";

        size_t st = (num * nodes) / 10;
        size_t en = ((num + 1) * nodes) / 10 - 1;

        cout << "Number of nodes mapped to " << num << " consumer: " << en - st + 1 << "\n";

        // now compute djisktra for each of this nodes to all other nodes and append to a file consumer<num>.txt

        for (size_t i = st; i <= en; i++)
        {
            // compute djisktra for node i
            djikstra(i, graph);
        }
        // cout << "Iteration " << iteration << " of consumer " << num << " done\n";
        file.close();
    }
    // two arguments first is key, second the number (which 1/10th of nodes mapped to it)
}
