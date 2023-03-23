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

	pthread_mutex_t room_mutex;

	void cleanRoom()
	{

		occupancy = 0;
		totalTime = 0;
		guest = -1;
		guestPriority = -1;
	}

	bool allotGuest(int32_t guest, int32_t priority)
	{

		if (occupancy == ROOM_SIZE)
		{

			printf("Guest %d could not be alloted this most suitable room since it's dirty\n", guest);
			return false;
		}

		else if (guestPriority > priority || (occupancy == ROOM_SIZE - 1 && guestPriority != -1))
		{

			printf("Guest %d could not be alloted this most suitable room due to priority or it's about to get dirty by this guest. "
				   "Room is already occupied by Guest %d\n",
				   guest, this->guest);
			return false;
		}

		auto tmp = this->guest;
		this->guest = guest;
		guestPriority = priority;

		printf("Guest %d was alloted ", this->guest);
		if (tmp != -1)
		{
			occupancy++;

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

		guest = -1;
		guestPriority = -1;
		occupancy++;
	}

	void updateTotalTime(int32_t time)
	{

		totalTime += time;
	}
} Room;

#endif