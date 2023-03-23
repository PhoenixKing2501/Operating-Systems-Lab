#include "Common.hpp"

void *cleanerThread(void *arg)
{
	auto id = *reinterpret_cast<int32_t *>(arg);
	delete reinterpret_cast<int32_t *>(arg);

	int32_t currentRoom{-1};
	for (;;)
	{

		printf("Cleaner %d waiting\n", id);

		sem_wait(&CleanerSem);

		pthread_mutex_lock(&hotel->cleaner_mutex);
		currentRoom = roomToClean;
		roomToClean++;
		pthread_mutex_unlock(&hotel->cleaner_mutex);

		printf("Cleaner %d started cleaning\n", id);

		sleep(hotel->rooms[currentRoom].totalTime);

		printf("Cleaner %d cleaning room %d\n", id, currentRoom);
		hotel->rooms[currentRoom].cleanRoom();

		pthread_mutex_lock(&hotel->cleaner_mutex);
		roomsCleaned++;
		pthread_mutex_unlock(&hotel->cleaner_mutex);

		pthread_mutex_lock(&hotel->cleaner_mutex);
		if (roomsCleaned == numRooms)
		{
			roomToClean = -1;
			roomsCleaned = -1;
			printf("Cleaners finished cleaning\n");

			for (int i = 0; i < numRooms * ROOM_SIZE; i++)
			{
				sem_post(&hotel->requestLeft);
			}
		}
		pthread_mutex_unlock(&hotel->cleaner_mutex);
	}

	return nullptr;
}
