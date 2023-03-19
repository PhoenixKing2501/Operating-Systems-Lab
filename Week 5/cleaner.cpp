#include "Common.hpp"

void *cleanerThread(void *arg)
{
	int id = pthread_self();
	int sval = -1;
	while (true)
	{
		pthread_mutex_lock(&hotel->cleaner_mutex);
	    do
		{
			sem_getvalue(&hotel->requestLeft, &sval);
			pthread_cond_wait(&hotel->cleaner_cond, &hotel->cleaner_mutex);
		} while (sval != 0); // to guard against spurious wakeups
		
		pthread_mutex_unlock(&hotel->cleaner_mutex);
		/**
		 * now start cleaning
		 */
		int32_t st = ((numRooms / numCleaners) * id);
		int32_t end = min(st + (numRooms / numCleaners), numRooms);
		/*it will clean sequentially the ith set of N/Y rooms from the start*/
		for (int i = st; i < end; i++)
		{
			int x = rand() % 10; // sleep for a time occupied by previous occupants multiplied by x
			sleep(hotel->rooms[i].totalTime * x);
			/*acquire a lock and increment counter*/
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
			// all rooms are cleaned
			// signal all guests by increase semaphore value
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
