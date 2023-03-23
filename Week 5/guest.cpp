#include "Common.hpp"

void *guestThread(void *arg)
{
	auto id = *static_cast<int32_t *>(arg);
	delete static_cast<int32_t *>(arg);
	int32_t roomNumber = -1, ret;

	printf("In guestThread %d\n", id);
	while (true)
	{
		sleep(rand() % REQTIME + 5);

		printf("Guest %d woke up\n", id);

		ret = sem_trywait(&hotel->requestLeft);

		printf("Guest %d requested a room\n", id);

		if (ret == -1)
		{
			if (errno == EAGAIN)
			{

				pthread_mutex_lock(&hotel->cleaner_mutex);
				if (roomToClean == -1)
				{
					roomToClean = 0;
					roomsCleaned = 0;
					pthread_mutex_unlock(&hotel->cleaner_mutex);
					for (int32_t i = 0; i < numRooms; i++)
					{
						sem_post(&CleanerSem);
					}
				}
				else
				{
					pthread_mutex_unlock(&hotel->cleaner_mutex);
				}
				sem_wait(&hotel->requestLeft);
			}
			else
			{
				perror("sem_trywait");
				exit(EXIT_FAILURE);
			}
		}
		roomToClean = -1;
		
		roomNumber = hotel->allotRoom(id, pr_guests[id]);
		if (roomNumber == -1)
		{

			printf("Guest %d could not be alloted a room\n", id);
			sem_post(&hotel->requestLeft);
			continue;
		}

		int32_t sleep_time = rand() % STAYTIME + 5;

		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += sleep_time;

		printf("Guest %d is starting to sleep\n", id);
		pthread_mutex_lock(&guest_mutex[id]);
		while (hotel->checkGuestInHotel(roomNumber, id))
		{

			ret = pthread_cond_timedwait(&guest_cond[id], &guest_mutex[id], &ts);
			if (ret == ETIMEDOUT)
			{

				hotel->checkoutGuest(roomNumber, sleep_time);
				printf("Guest %d successfully completed it's stay\n", id);
				break;
			}
			else if (ret == 0)
			{

				hotel->updateTotalTime(roomNumber, sleep_time - (ts.tv_sec - time(NULL)));
				printf("Guest %d was kicked out by another guest\n", id);
				break;
			}
		}

		pthread_mutex_unlock(&guest_mutex[id]);
	}

	return NULL;
}