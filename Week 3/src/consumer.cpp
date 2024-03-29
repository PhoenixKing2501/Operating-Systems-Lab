#include <bits/stdc++.h>

#include "Graph.hpp"
using namespace std;

int32_t num;
int32_t iteration = 0;
ofstream file;
/*file to write paths of this consumer*/
ofstream time_file;
/*file to print time every 5 iterations*/
size_t nodes;
/*number of nodes in graph currently*/
size_t old_size = 0;
/*store old number of nodes in graph*/
size_t st, en;
/*start and end indexes for new nodes assigned to this consumer*/

vector<vector<int8_t>> dist_mat(VEC_LEN, vector<int8_t>(VEC_LEN, numeric_limits<int8_t>::max()));
vector<vector<vector<size_t>>> path_mat(VEC_LEN, vector<vector<size_t>>(VEC_LEN, vector<size_t>()));

vector<size_t> old_nodes;
/*vector to store old nodes for this consumer process*/
vector<size_t> new_nodes;
/*vector to store new nodes for this consumer process*/

void dijkstra_opt(size_t src, Graph<bool> &graph)
{
    priority_queue<pair<int8_t, size_t>, vector<pair<int8_t, size_t>>, greater<pair<int8_t, size_t>>> pq;
    vector<int8_t> dist(nodes, numeric_limits<int8_t>::max());
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
    }

    for (size_t i = 0; i < nodes; i++)
    {
        dist_mat[src][i] = dist_mat[i][src] = dist[i];
        // if dist[i] == numeric_limits<int8_t>::max() then there is no path from src to i
        if (dist[i] == numeric_limits<int8_t>::max() || dist[i] == 0)
            continue;
        vector<size_t> path;
        for (size_t j = i; j != src; j = last_visitied[j])
        {
            path.push_back(j);
        }
        path.push_back(src);
        path_mat[src][i].clear();
        copy(path.rbegin(), path.rend(), back_inserter(path_mat[src][i]));
        path_mat[i][src].clear();
        copy(path.begin(), path.end(), back_inserter(path_mat[i][src]));
    }
}

void dijkstra(size_t src, Graph<bool> &graph)
{
    priority_queue<pair<int8_t, size_t>, vector<pair<int8_t, size_t>>, greater<pair<int8_t, size_t>>> pq;
    vector<int8_t> dist(nodes, numeric_limits<int8_t>::max());
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
    }

    for (size_t i = 0; i < nodes; i++)
    {
        // if dist[i] == numeric_limits<int8_t>::max() then there is no path from src to i
        if (dist[i] == numeric_limits<int8_t>::max() || dist[i] == 0)
            continue;
        vector<size_t> path;
        for (size_t j = i; j != src; j = last_visitied[j])
        {
            path.push_back(j);
        }
        file << src << " " << i << ":" << static_cast<int>(dist[i]) << ":";
        path.push_back(src);
        reverse(path.begin(), path.end());
        for (auto &k : path)
        {
            file << k << " ";
        }
        file << "\n";
    }
}
void optimize(Graph<bool> &graph)
{

    for (size_t i = old_size; i < nodes; ++i)
    {
        // compute dijkstra on all the new nodes
        dijkstra_opt(i, graph);
    }
    for (auto &i : old_nodes)
    {
        for (size_t j = 0; j < nodes; j++)
        {
            /* compute path from mapped nodes to all pre-existing nodes */
            /* if path shortened by introduction of any of the new nodes, update */

            long int conn_newnode = -1;
            for (size_t k = old_size; k < nodes; k++)
            {
                if (dist_mat[i][k] + dist_mat[k][j] < dist_mat[i][j])
                {
                    conn_newnode = k;
                }
            }
            if (conn_newnode != -1)
            {
                dist_mat[i][j] = dist_mat[i][conn_newnode] + dist_mat[conn_newnode][j];
                path_mat[i][j].clear();
                copy(path_mat[i][conn_newnode].begin(), path_mat[i][conn_newnode].end(), back_inserter(path_mat[i][j]));
                path_mat[i][j].push_back(conn_newnode);
                copy(path_mat[conn_newnode][j].begin(), path_mat[conn_newnode][j].end(), back_inserter(path_mat[i][j]));
            }
        }
    }
    // fill the new nodes vector
    for (size_t i = st; i <= en; ++i)
    {
        new_nodes.push_back(i);
    }

    old_nodes.insert(old_nodes.end(), new_nodes.begin(), new_nodes.end());
    new_nodes.clear();
    for (auto &i : old_nodes)
    {
        for (size_t j = 0; j < nodes; j++)
        {
            if (i == j)
                continue;
            file << i << " " << j << ":" << static_cast<int>(dist_mat[i][j]) << ":";
            for (auto &x : path_mat[i][j])
            {
                file << x << " ";
            }
            file << "\n";
        }
    }

    old_size = nodes;
}
void unoptimize(Graph<bool> &graph)
{
    for (size_t i = 0; i < nodes; ++i)
    {
        // compute dijkstra on all the nodes
        dijkstra(i, graph);
    }
}
int main(int argc, char const *argv[])
{
    bool opt_flag = false;
    if (argc < 3)
    {
        cout << "Usage: ./consumer <shmkey> <number> [options]\n";
        // three arguments first is key, second the number (which 1/10th of nodes mapped to it),third the option
        exit(EXIT_FAILURE);
    }
    else if (argc == 4)
    {
        if (strcmp(argv[3], "-optimize") == 0)
        {
            opt_flag = true;
        }
        else
        {
            cout << "Invalid option\n";
            exit(EXIT_FAILURE);
        }
    }
    num = stoi(argv[2]);
    Graph<bool> graph(stoi(argv[1]));
    while (true)
    {
        this_thread::sleep_for(chrono::seconds(30));
        iteration++;

        string filename = "consumer" + to_string(num) + ".txt";
        file.open(filename, ios::out | ios::app);

        /*get total nodes in graph*/
        nodes = graph.getSize();
        /*get total new nodes created in the graph*/
        size_t new_node_cnt = nodes - old_size;

        /*calculate start and end index of new nodes for this consumer process*/
        st = old_size + (num * new_node_cnt) / 10;
        en = old_size + ((num + 1) * new_node_cnt) / 10 - 1;

        file << "Iteration: " << iteration << "\n";
        cout << "Number of nodes in graph: " << nodes << "\n";
        cout << "Number of new nodes mapped to " << num << " consumer: " << en - st + 1;

        if (opt_flag)
        {
            optimize(graph);
        }
        else
        {
            unoptimize(graph);
        }

        file.close();
        /*calculate running time for consumer every 5 iterations
        if (iteration % 5 == 0)
        {
            ofstream time_file;
            string time_filename = "consumer" + to_string(num) + "time.txt";
            time_file.open(time_filename, ios::out | ios::app);
            int pid = getpid();
            char dir_name[100];
            int non_sleep_time;
            int utime, stime, cutime, cstime;
            sprintf(dir_name, "/proc/%d/stat", pid);
            FILE *fp = fopen(dir_name, "r");
            if (fp == NULL)
            {
                //throw error file not found
                throw std::runtime_error("File not found");
            }
            if (fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d "
                           "%*u %*u %*u %*u %*u %d %d %d %d",
                       &utime, &stime, &cutime, &cstime) == 4)
            {
                non_sleep_time = utime + stime;
                fclose(fp);
            }
            else
            {
                throw std::runtime_error("Error in reading");
            }
            time_file << "Iteration:" << iteration << ":" << non_sleep_time << "\n";
            time_file.close();
        }*/
    }
}
