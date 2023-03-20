#pragma once

#ifndef _ROOM_HPP_
#define _ROOM_HPP_

#include "Common.hpp"

struct Room
{
	int32_t occupancy{};
	int32_t totalTime{};

	int32_t guest{-1};
	int32_t guestPriority{-1};

	pthread_mutex_t roomMutex;

	Room()
	{
		pthread_mutex_init(&roomMutex, NULL);
	}
	~Room()
	{
		pthread_mutex_destroy(&roomMutex);
	}

	bool allotGuest(int32_t guest, int32_t priority)
	{
		pthread_mutex_lock(&roomMutex);
		if (occupancy == ROOM_SIZE)
		{
			pthread_mutex_unlock(&roomMutex);
			return false;
		}

		else if (guestPriority > priority)
		{
			pthread_mutex_unlock(&roomMutex);
			return false;
		}

		auto tmp = this->guest;
		this->guest = guest;
		guestPriority = priority;
		occupancy++;
		pthread_mutex_unlock(&roomMutex);
		// signal the previous thread
		pthread_cond_signal(&guest_cond[tmp]);

		cout << "Guest " << this->guest << "  was alloted ";
		if (tmp != -1)
		{
			cout << "a room replacing Guest " << tmp << "\n";
		}
		else
		{
			cout << "a clean room\n";
		}
		return true;
	}

	void checkoutGuest(int32_t time)
	{
		pthread_mutex_lock(&roomMutex);
		totalTime += time;
		guest = -1;
		guestPriority = -1;
		pthread_mutex_unlock(&roomMutex);
	}


};

#endif // _ROOM_HPP_
