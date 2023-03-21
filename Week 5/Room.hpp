#pragma once

#ifndef _ROOM_HPP_
#define _ROOM_HPP_

#include "Common.hpp"

typedef struct Room
{
	int32_t occupancy{};
	int32_t totalTime{};

	int32_t guest{-1};
	int32_t guestPriority{-1};

	pthread_mutex_t roomMutex;

	Room()
	{
		pthread_mutex_init(&roomMutex, nullptr);
	}
	~Room()
	{
		pthread_mutex_destroy(&roomMutex);
	}

	void cleanRoom()
	{
		pthread_mutex_lock(&roomMutex);
		occupancy = 0;
		totalTime = 0;
		guest = -1;
		guestPriority = -1;
		pthread_mutex_unlock(&roomMutex);
	}

	bool allotGuest(int32_t guest, int32_t priority)
	{
		pthread_mutex_lock(&roomMutex);
		if (occupancy == ROOM_SIZE)
		{
			pthread_mutex_unlock(&roomMutex);
			printf("Guest %d could not be alloted this most suitable room since it's dirty\n", guest);
			return false;
		}

		else if (guestPriority > priority || (occupancy == ROOM_SIZE - 1 && guestPriority != -1))
		{
			pthread_mutex_unlock(&roomMutex);
			printf("Guest %d could not be alloted this most suitable room due to priority or it's about to get dirty by this guest. "
				   "Room is already occupied by Guest %d\n",
				   guest, this->guest);
			return false;
		}

		// signal the previous thread
		auto tmp = this->guest;
		this->guest = guest;
		guestPriority = priority;
		pthread_mutex_unlock(&roomMutex);

		printf("Guest %d was alloted ", this->guest);
		if (tmp != -1)
		{
			occupancy++; // this is being done for the previous guest

			printf("a room replacing Guest %d\n", tmp);
			pthread_cond_signal(&guest_cond[tmp]);
		}
		else
		{

			printf("a clean room\n");
		}
		return true;
	}

	void checkoutGuest(int32_t time)
	{
		updateTotalTime(time);
		pthread_mutex_lock(&roomMutex);
		guest = -1;
		guestPriority = -1;
		occupancy++;
		pthread_mutex_unlock(&roomMutex);
	}

	void updateTotalTime(int32_t time)
	{
		pthread_mutex_lock(&roomMutex);
		totalTime += time;
		pthread_mutex_unlock(&roomMutex);
	}
} Room;

#endif // _ROOM_HPP_
