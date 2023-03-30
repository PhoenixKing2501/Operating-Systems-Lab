#include "Common.hpp"

void *guestThread(void *arg)
{
	auto id = *static_cast<int32_t *>(arg);
	delete static_cast<int32_t *>(arg);
	int32_t roomNumber = -1, ret = 0;
	int sval{};

	printf("In guestThread %d\n", id);
	while (true)
	{
		sleep(rand() % REQTIME + 10);

		printf("Guest %d woke up and requested a room\n", id);

		pthread_mutex_lock(&hotel->cleaner_mutex);

		sem_getvalue(&hotel->requestLeft, &sval);

		if (sval == 0)
		{
			if (hotel->roomToClean == -1)
			{
				hotel->roomToClean = 0;
				hotel->roomsCleaned = 0;

				for (int32_t i = 0; i < numGuests; i++)
				{
					pthread_cond_signal(&guest_cond[i]);
				}
				pthread_mutex_unlock(&hotel->cleaner_mutex);
				for (int32_t i = 0; i < numRooms; i++)
				{
					sem_post(&hotel->CleanerSem);
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
			pthread_mutex_unlock(&hotel->cleaner_mutex);
		}

		roomNumber = hotel->allotRoom(id, pr_guests[id]);
		if (roomNumber == -1)
		{
			continue;
		}

		sem_wait(&hotel->requestLeft);

		int32_t sleep_time = rand() % STAYTIME + 10;

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
		}

		pthread_mutex_unlock(&guest_mutex[id]);
		if (ret == 0)
		{

			hotel->updateTotalTime(roomNumber, sleep_time - (ts.tv_sec - time(NULL)));
			printf("Guest %d was kicked out\n", id);
		}
	}

	return NULL;
}