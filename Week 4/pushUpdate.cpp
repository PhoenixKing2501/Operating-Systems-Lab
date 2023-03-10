#include "Common.hpp"

/* monitor shared queue,if new element found implement by
 * pusing updates to feed queue of neighbours of node
 */

void *pushUpdateRunner(void *_num)
{
	int num = *reinterpret_cast<int *>(_num);
	delete reinterpret_cast<int *>(_num);
	ostringstream buf;
	for (;;)
	{
		auto action = shared_queue.pop(num);

		string type{};
		if (action.type == Action::Type::Post)
		{
			type = "Post";
		}
		else if (action.type == Action::Type::Comment)
		{
			type = "COMMENT";
		}
		else if (action.type == Action::Type::Like)
		{
			type = "LIKE";
		}

		buf << "Pushing update of " << action.user_id
			<< " with action id " << action.action_id
			<< ", type " << type
			<< " posted at time " << get_time(action.timestamp)
			<< "\n";

		fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), fptr);
		fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), stdout);

		buf.str("");
		buf.clear();

		for (auto &&node : *nodes[action.user_id].neighbors)
		{
			updates.push(node);

			buf << "Action " + type + ", user_id: " << action.user_id
				<< ", Neighbour: " << node
				<< "\n";

			fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), fptr);
			fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), stdout);

			buf.str("");
			buf.clear();

			pthread_mutex_lock(&nodes[node].feedQueue_mutex);
			nodes[node].pushToFeed(action);
			pthread_mutex_unlock(&nodes[node].feedQueue_mutex);
		}
	}
}
