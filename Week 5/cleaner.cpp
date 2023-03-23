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

		printf("Cleaner %d waiting for all rooms to be cleaned\n", id);
		pthread_mutex_lock(&hotel->cleaner_mutex);
		sem_getvalue(&hotel->requestLeft, &sval);

		while (sval != 0)
		{
			pthread_cond_wait(&hotel->cleaner_cond, &hotel->cleaner_mutex);
			sem_getvalue(&hotel->requestLeft, &sval);
		}

		pthread_mutex_unlock(&hotel->cleaner_mutex);

		printf("Cleaner %d started cleaning\n", id);

		for (int32_t i = st; i < end; ++i)
		{

			sleep(hotel->rooms[i].totalTime);

			printf("Cleaner %d cleaning room %d\n", id, i);
			hotel->rooms[i].cleanRoom();

			pthread_mutex_lock(&hotel->cleaner_mutex);
			cleaner_ctr++;
			pthread_mutex_unlock(&hotel->cleaner_mutex);
		}

		pthread_mutex_lock(&hotel->cleaner_mutex);
		if (cleaner_ctr == numRooms)
		{
			cleaner_ctr = 0;
			pthread_cond_signal(&hotel->cleaner_cond);
			pthread_mutex_unlock(&hotel->cleaner_mutex);

			printf("Cleaners finished cleaning\n");

			for (int i = 0; i < numRooms * ROOM_SIZE; i++)
			{
				sem_post(&hotel->requestLeft);
			}
		}
		else
		{

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
