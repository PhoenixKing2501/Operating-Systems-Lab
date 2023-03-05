#include "Common.hpp"
#include "Graph.hpp"
#include "Node.hpp"

#include <bits/stdc++.h>
#include <chrono>

using namespace std;

/*monitor shared queue,if new element found implement by pusing updates to feed queue of neighbours of node*/

/*parameter for this thread is graph*/

void *pushUpdateRunner(
	void *_graph)
{
	Graph<Node> &graph = *reinterpret_cast<Graph<Node> *>(_graph);
	ostringstream buf;
	while (true)
	{
		// FILE *fptr = fopen("sns.log", "a");
		pthread_mutex_lock(&shared_queue_mutex);
		while (shared_queue.empty())
		{
			pthread_cond_wait(&shared_queue_cond, &shared_queue_mutex);
		}

		Action action = shared_queue.front();
		shared_queue.pop();
		pthread_mutex_unlock(&shared_queue_mutex);
		string type{};
		if (action.type == Action::Type::Post)
		{
			type = "Post";
		}
		else if (action.type == Action::Type::Comment)
		{
			type = "Comment";
		}
		else if (action.type == Action::Type::Like)
		{
			type = "Like";
		}

		/*print the debug messages to terminal and sns.log*/
		/*convert timestamp to readable format*/

		buf << "Pushing update of " << action.user_id
			<< " with action id " << action.action_id
			<< ", type " << type
			<< " posted at time " << get_time(action.timestamp) << "\n";

		// pthread_mutex_lock(&print_mutex);
		fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), fptr);
		fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), stdout);
		// pthread_mutex_unlock(&print_mutex);

		buf.str("");
		buf.clear();

		for (auto &&node : graph.adjList[action.user_id])
		{
			buf << "Action user_id: " << action.user_id
				<< ", Neighbour: " << node
				<< "\n";

			// pthread_mutex_lock(&print_mutex);
			fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), fptr);
			fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), stdout);
			// pthread_mutex_unlock(&print_mutex);

			buf.str("");
			buf.clear();
			// push to feed queue of node
			pthread_mutex_lock(&nodes[node].feedQueue_mutex);
			nodes[node].pushToFeed(action);
			// pthread_cond_broadcast(&nodes[node].feedQueue_cond);
			pthread_cond_signal(&nodes[node].feedQueue_cond);
			pthread_mutex_unlock(&nodes[node].feedQueue_mutex);
			/*keep the cond_signal outside mutex lock and observe behaviour*/
			/*use broadcast and observe behaviour*/
		}
		// fclose(fptr);
	}
}
