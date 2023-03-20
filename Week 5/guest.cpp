#include "Common.hpp"

void *guestThread(void *arg)
{
	int id = pthread_self();
	while (1)
	{
		sleep(rand() % 10 + 10); /*sleep for a random time between 10 and 20 seconds*/
		/*try to occupy a room*/
		int ret = sem_trywait(&hotel->requestLeft);
		cout << "Guest " << id << "  requested a room\n";
		if (ret == -1)
		{
			if (errno == EAGAIN)
			{
				/*no room can be occupied,has to clean now*/
				pthread_cond_signal(&hotel->cleaner_cond);
				sem_wait(&hotel->requestLeft);
			}
			else
			{
				perror("sem_trywait");
				exit(EXIT_FAILURE);
			}
		}
		/*call allotRoom to try to allot to this guest*/
		if (not hotel->allotRoom(id, pr_guests[0]))
		{
			/*allotment failed,now released */
			cout << "Guest " << id << "  could not be alloted a room\n";
			sem_post(&hotel->requestLeft);
			continue;
		}
		/*allotment successful,now occupied*/
		/*now conditionally sleep and wait for random time*/
		int32_t sleep_time = rand() % 20 + 10;

		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += sleep_time;

		
		pthread_mutex_lock(&guest_mutex[id]);
		while (hotel->checkGuestInHotel(id)) // to guard against spurious wakeups
		{
			/*sleep and wait*/
			ret = pthread_cond_timedwait(&guest_cond[id], &guest_mutex[id], &ts);
		}

		pthread_mutex_unlock(&guest_mutex[id]);

		if (ret == -1 && errno == ETIMEDOUT)
		{
			// guest successfully slept for sleep_time seconds
			cout << "Guest " << id << " successfully completed it's stay\n";
			hotel->updateTotalTime(id, sleep_time);
		}
		else
		{
			// guest was kicked out by another guest
			// update time by the time he slept
			cout << "Guest " << id << "  was kicked out by another guest\n";
			hotel->updateTotalTime(id, sleep_time - (ts.tv_sec - time(NULL)));
		}
	}

	return NULL;
}
