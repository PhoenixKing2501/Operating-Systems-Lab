#include "Common.hpp"

void *cleanerThread(void *arg)
{
	auto id = *((size_t *)arg);

	delete (size_t *)arg;
	int sval = -1;

	while (true)
	{
		cout << "Cleaner " << id << " waiting for all rooms to be cleaned\n";
		pthread_mutex_lock(&hotel->cleaner_mutex);
		sem_getvalue(&hotel->requestLeft, &sval);

		while (sval != 0) // to guard against spurious wakeups
		{
			pthread_cond_wait(&hotel->cleaner_cond, &hotel->cleaner_mutex);
			sem_getvalue(&hotel->requestLeft, &sval);
		}

		pthread_mutex_unlock(&hotel->cleaner_mutex);
		cout << "Cleaner " << id << " started cleaning\n";
		// now start cleaning

		int32_t st = ((numRooms / numCleaners) * id);
		int32_t end = (id + 1 == numCleaners) ? numRooms : (numRooms / numCleaners) * (id + 1);

		/*it will clean sequentially the ith set of N/Y rooms from the start*/
		/*can lead to an uneven sleep distribution,think of distributing according to sleep time*/

		for (int i = st; i < end; i++)
		{
			int x = rand() % 10; // sleep for a time occupied by previous occupants multiplied by x
			sleep(hotel->rooms[i].totalTime * x);
			/*acquire a lock and update room detail and increment counter*/
			/*every cleaner can have a mutex here to achieve higher parallelism*/
			cout << "Cleaner " << id << " cleaned room " << i + 1 << "\n";

			pthread_mutex_lock(&hotel->cleaner_mutex);
			hotel->rooms[i].occupancy = 0;
			hotel->rooms[i].guest = 0;
			hotel->rooms[i].guestPriority = -1;
			cleaner_ctr++;

			pthread_mutex_unlock(&hotel->cleaner_mutex);
		}

		// acquire lock again and check if all rooms are cleaned by cleaner_ctr
		pthread_mutex_lock(&hotel->cleaner_mutex);
		if (cleaner_ctr == numRooms * ROOM_SIZE)
		{
			cout << "Cleaners finished cleaning\n";
			// all rooms are cleaned
			// signal all guests by increasing semaphore value
			for (int i = 0; i < numRooms * ROOM_SIZE; i++)
			{
				sem_post(&hotel->requestLeft);
			}
			cleaner_ctr = 0;
		}
		pthread_mutex_unlock(&hotel->cleaner_mutex);
	}
	return NULL;
}
