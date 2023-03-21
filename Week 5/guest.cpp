#include "Common.hpp"

void *guestThread(void *arg)
{
	auto id = *static_cast<int32_t *>(arg);
	delete static_cast<int32_t *>(arg);
	int32_t roomNumber = -1, ret;
	// cout << "In guestThread " << id << "\n";
	printf("In guestThread %d\n", id);
	while (true)
	{
		sleep(rand() % REQTIME + 5); /*sleep for a random time between 10 and 20 seconds*/
		// cout << "Guest " << id << " woke up\n";
		printf("Guest %d woke up\n", id);
		/*try to occupy a room*/
		ret = sem_trywait(&hotel->requestLeft);
		// cout << "Guest " << id << " requested a room\n";
		printf("Guest %d requested a room\n", id);

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
		roomNumber = hotel->allotRoom(id, pr_guests[id]);
		if (roomNumber == -1)
		{
			/*allotment failed,now released */
			// cout << "Guest " << id << " could not be alloted a room\n";
			printf("Guest %d could not be alloted a room\n", id);
			sem_post(&hotel->requestLeft);
			continue;
		}
		/*allotment successful,now occupied*/
		/*now conditionally sleep and wait for random time*/
		int32_t sleep_time = rand() % STAYTIME + 5;

		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += sleep_time;

		// cout << "Guest " << id << " is starting to sleep\n";
		printf("Guest %d is starting to sleep\n", id);
		pthread_mutex_lock(&guest_mutex[id]);
		while (hotel->checkGuestInHotel(roomNumber, id)) // to guard against spurious wakeups
		{
			/*sleep and wait*/
			ret = pthread_cond_timedwait(&guest_cond[id], &guest_mutex[id], &ts);
			if (ret == ETIMEDOUT)
			{
				// guest successfully slept for sleep_time seconds
				// cout << "Guest " << id << " successfully completed it's stay\n";
				printf("Guest %d successfully completed it's stay\n", id);
				hotel->checkoutGuest(roomNumber ,sleep_time);
				break;
			}
			else if (ret == 0)
			{
				// guest was kicked out by another guest
				// update time by the time he slept
				// cout << "Guest " << id << " was kicked out by another guest\n";
				printf("Guest %d was kicked out by another guest\n", id);
				hotel->updateTotalTime(roomNumber,sleep_time - (ts.tv_sec - time(NULL)));
				break;
			}
		}

		pthread_mutex_unlock(&guest_mutex[id]);
	}

	return NULL;
}
