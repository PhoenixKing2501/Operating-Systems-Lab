#include "Common.hpp"

/* make a runner function for threads [total 10 threads],
 * each function is assigned a set of feed queues,range in parameter
 * each function is assigned 3,770 feed queues on which it
 * prints in order stored in it's priority queue
 */

void *readPostRunner(void *param)
{
	/*param is actually an integer pointer, extract it*/
	int num = *reinterpret_cast<int *>(param);
	delete reinterpret_cast<int *>(param);

	/*monitor for feed queues for nodes in the range st to end using pthead_cond_wait*/
	for (;;)
	{
		auto &node = nodes[updates.pop(num)];

		/*print the feed queue in order*/
		pthread_mutex_lock(&node.feedQueue_mutex);
		while (not node.feedQueue.empty())
		{
			Action action = node.popFromFeed();

			string type{};
			if (action.type == Action::Type::Post)
			{
				type = "POST";
			}
			else if (action.type == Action::Type::Comment)
			{
				type = "COMMENT";
			}
			else if (action.type == Action::Type::Like)
			{
				type = "LIKE";
			}

			ostringstream buf;
			buf << "I (user " << node.id << ")"
				<< " read action number " << action.action_id
				<< " of type " << type
				<< " posted by user " << action.user_id
				<< " at time " << get_time(action.timestamp)
				<< "\n";

			fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), fptr);
			fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), stdout);

			buf.str("");
			buf.clear();
		}
		pthread_mutex_unlock(&node.feedQueue_mutex);
	}
}
