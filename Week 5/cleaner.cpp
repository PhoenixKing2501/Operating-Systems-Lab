#include "Common.hpp"

void *cleanerThread(void *arg)
{
	auto id = *reinterpret_cast<int32_t *>(arg);
	delete reinterpret_cast<int32_t *>(arg);

	int32_t st = ((numRooms / numCleaners) * id);
	int32_t end = (id + 1 == numCleaners)
					  ? numRooms
					  : (numRooms / numCleaners) * (id + 1);

	int sval{};

	for (;;)
	{
		// cout << "Cleaner " << id << " waiting for all rooms to be cleaned\n";
		printf("Cleaner %d waiting for all rooms to be cleaned\n", id);
		pthread_mutex_lock(&hotel->cleaner_mutex);
		sem_getvalue(&hotel->requestLeft, &sval);

		while (sval != 0) // to guard against spurious wakeups
		{
			pthread_cond_wait(&hotel->cleaner_cond, &hotel->cleaner_mutex);
			sem_getvalue(&hotel->requestLeft, &sval);
		}

		pthread_mutex_unlock(&hotel->cleaner_mutex);
		// cout << "Cleaner " << id << " started cleaning\n";
		printf("Cleaner %d started cleaning\n", id);
		// now start cleaning

		/*it will clean sequentially the ith set of N/Y rooms from the start*/
		/*can lead to an uneven sleep distribution,think of distributing according to sleep time*/

		for (int32_t i = st; i < end; ++i)
		{
			// int x = rand() % 10; // sleep for a time occupied by previous occupants multiplied by x
			sleep(hotel->rooms[i].totalTime);
			/*acquire a lock and update room detail and increment counter*/
			/*every cleaner can have a mutex here to achieve higher parallelism*/
			// cout << "Cleaner " << id << " cleaned room " << i + 1 << "\n";
			printf("Cleaner %d cleaning room %d\n", id, i);
			hotel->rooms[i].cleanRoom();

			pthread_mutex_lock(&hotel->cleaner_mutex);
			cleaner_ctr++;
			pthread_mutex_unlock(&hotel->cleaner_mutex);
		}

		// acquire lock again and check if all rooms are cleaned by cleaner_ctr
		pthread_mutex_lock(&hotel->cleaner_mutex);
		if (cleaner_ctr == numRooms)
		{
			cleaner_ctr = 0;
			pthread_cond_signal(&hotel->cleaner_cond);
			pthread_mutex_unlock(&hotel->cleaner_mutex);
			// cout << "Cleaners finished cleaning\n";
			printf("Cleaners finished cleaning\n");
			// all rooms are cleaned
			// signal all guests by increasing semaphore value
			for (int i = 0; i < numRooms * ROOM_SIZE; i++)
			{
				sem_post(&hotel->requestLeft);
			}
		}
		else
		{
			// cout << "Cleaner " << id << " waiting for other cleaners to finish\n";
			printf("Cleaner %d waiting for other cleaners to finish\n", id);
			while (cleaner_ctr != 0)
			{
				pthread_cond_wait(&hotel->cleaner_cond, &hotel->cleaner_mutex);
			}
			pthread_mutex_unlock(&hotel->cleaner_mutex);
		}
	}

	return nullptr;
}
