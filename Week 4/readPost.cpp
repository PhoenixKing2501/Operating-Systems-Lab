#include "Common.hpp"

/*make a runner function for threads [total 10 threads],each function is assigned a set of feed queues,range in parameter
  each function is assigned 3,770 feed queues on which it prints in order stored in it's priority queue*/

void *readPostRunner(void *param)
{
	/*param is actually an integer pointer, extract it*/
	int num = *reinterpret_cast<int *>(param);
	delete reinterpret_cast<int *>(param);
	// size_t st = num * static_cast<size_t>(3770);
	// size_t end = st + 3770;
	/*monitor for feed queues for nodes in the range st to end using pthead_cond_wait*/
	for (;;)
	{
		// FILE *fname = fopen("sns.log", "a");
		// for (size_t i = st; i < end; i++)
		// {
		// while (nodes[i].feedQueue.empty())
		// {
		// 	pthread_cond_wait(&nodes[i].feedQueue_cond, &nodes[i].feedQueue_mutex);
		// }

		auto _node = updates.pop(num);
		if (not _node)
		{
			this_thread::sleep_for(chrono::milliseconds(100));
			continue;
		}

		auto &node = nodes[_node.value()];

		/*print the feed queue in order*/
		pthread_mutex_lock(&node.feedQueue_mutex);
		while (not node.feedQueue.empty())
		{
			// cout << nodes[i].feedQueue.top().timestamp << " ";
			// fwrite to file fname in main.cpp
			// create a string of type
			// I read action number XX of type YY posted by user ZZ at time TT
			// where XX is the action id, YY is the type of action, ZZ is the user id, and TT is the timestamp
			// write to file
			// pop Action from top of feed queue and extract these

			Action a = node.popFromFeed();

			string type{};
			if (a.type == Action::Type::Post)
			{
				type = "POST";
			}
			else if (a.type == Action::Type::Comment)
			{
				type = "COMMENT";
			}
			else if (a.type == Action::Type::Like)
			{
				type = "LIKE";
			}
			// string s = "I read action number " + to_string(a.action_id) +
			// 		   " of type " + type +
			// 		   " posted by user " + to_string(a.user_id) +
			// 		   " at time " + to_string(a.timestamp) +
			// 		   "in feed of user " + to_string(i) +
			// 		   "\n";
			// do as rounak did
			ostringstream buf;
			buf << "I (user " << node.id << ")"
				<< " read action number " << a.action_id
				<< " of type " << type
				<< " posted by user " << a.user_id
				<< " at time " << get_time(a.timestamp)
				<< "\n";

			// pthread_mutex_lock(&print_mutex);
			fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), fptr);
			// write the same to stdout using fwrite
			fwrite(buf.str().c_str(), sizeof(char), buf.str().length(), stdout);
			// pthread_mutex_unlock(&print_mutex);

			buf.str("");
			buf.clear();
			/*opened closed several times,not efficient but atleast need to see live output*/
			// if a.type is comment then type is "comment" and so on
			// }
			// pthread_mutex_unlock(&nodes[i].feedQueue_mutex);
			// }
			// fclose(fname);
		}
		pthread_mutex_unlock(&node.feedQueue_mutex);
	}
}
