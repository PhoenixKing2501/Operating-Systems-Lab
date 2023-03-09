#include "Common.hpp"

vector<size_t> get_uniq_rand_set(
	size_t min,
	size_t max,
	size_t count)
{
	// size_t left = count;
	size_t range = max - min + 1;
	static mt19937_64 rng{random_device{}()};

	vector<size_t> posn(range);
	iota(posn.begin(), posn.end(), min);

	vector<size_t> ret{};

	while (count--)
	{
		uniform_int_distribution<size_t> dist{0, range - 1};
		size_t index = dist(rng);
		ret.push_back(posn[index]);
		swap(posn[index], posn[range - 1]);
		range--;
	}

	return ret;
}

void *userSimulatorRunner(void *ptr)
{
	Graph<size_t> &graph = *reinterpret_cast<Graph<size_t> *>(ptr);
	ostringstream buf;

	array type_string{"POST", "COMMENT", "LIKE"};

	size_t counter{};

	for (;;)
	{
		// FILE *fptr = fopen("sns.log", "a");

		cerr << "User Simulator Iter: " << (++counter) << endl;

		vector<size_t> &&active_nodes = get_uniq_rand_set(0, graph.getSize() - 1, 100);

		buf << "Random nodes selected by userSimulator: ";
		for (auto &node : active_nodes)
		{
			buf << node << " ";
		}
		buf << "\n";

		// pthread_mutex_lock(&print_mutex);
		fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), fptr);
		fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), stdout);
		// pthread_mutex_unlock(&print_mutex);

		buf.str("");
		buf.clear();

		for (auto &node : active_nodes)
		{
			long action_count = lround(log2(graph.getDegree(node) + 1));
			buf << "User " << node
				<< " (degree: " << graph.getDegree(node) << ") performs "
				<< action_count << " actions\n";

			// pthread_mutex_lock(&print_mutex);
			fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), fptr);
			fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), stdout);
			// pthread_mutex_unlock(&print_mutex);

			buf.str("");
			buf.clear();

			for (long i = 0; i < action_count; ++i)
			{
				Action &&action = nodes[node].genNextAction();
				/*push to shared queue and pthread_cond_signal that this queue has changed and lock before pushing*/
				// cerr << action.user_id << endl;
				// pthread_mutex_lock(&shared_queue_mutex);
				shared_queue.push(action);
				// pthread_mutex_unlock(&shared_queue_mutex);
				// pthread_cond_signal(&shared_queue_cond);
				nodes[node].pushToWall(action);
				/* push to global queue accessed by pushUpdate : remaining part, discussion needed on the structure of the global queue */

				buf << "User " << node
					<< " performs " << type_string[static_cast<int>(action.type)]
					<< " no. " << action.action_id
					<< "\n";

				// pthread_mutex_lock(&print_mutex);
				fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), fptr);
				fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), stdout);
				// pthread_mutex_unlock(&print_mutex);

				buf.str("");
				buf.clear();
			}
		}

		// fclose(fptr);
		this_thread::sleep_for(chrono::seconds(120));
		// this_thread::sleep_for(chrono::seconds(1));
		// sleep(120);
	}
}
