#pragma once

#ifndef _HOTEL_HPP_
#define _HOTEL_HPP_

#include "Common.hpp"

struct Hotel
{
	vector<Room> rooms{};
	vector<pthread_t> cleaners{};
	sem_t requestLeft;
	pthread_cond_t cleaner_cond;
	pthread_mutex_t cleaner_mutex;

	Hotel(size_t X, size_t N)
		: rooms(N), cleaners(X)
	{
		sem_init(&requestLeft, 0, ROOM_SIZE * N);

		for (size_t i = 0; i < X; i++)
		{
			pthread_create(&cleaners[i], NULL, cleanerThread, NULL);
		}
	}

	~Hotel()
	{
		// join all cleaners
		for (size_t i = 0; i < cleaners.size(); i++)
		{
			pthread_join(cleaners[i], NULL);
		}
		sem_destroy(&requestLeft);
	}

	bool allotRoom(pthread_t guest, int32_t priority)
	{
		for (size_t i = 0; i < rooms.size(); i++)
		{
			if (rooms[i].allotGuest(guest, priority))
			{
				return true;
			}
		}

		return false;
	}
	// can speed up both these functions
	void updateTotalTime(pthread_t gid, int32_t time)
	{
		for (size_t i = 0; i < rooms.size(); i++)
		{
			/*check if guest assigned this room*/
			if (rooms[i].guest == gid)
			{
				rooms[i].updateTotalTime(time);
				break;
			}
		}
	}
	bool checkGuestInHotel(pthread_t gid)
	{
		for (size_t i = 0; i < rooms.size(); i++)
		{
			if (rooms[i].guest == gid)
			{
				return true;
			}
		}
		return false;
	}
};

#endif // _HOTEL_HPP_