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

	Hotel(int32_t X, int32_t N)
	{
		sem_init(&requestLeft, 0, ROOM_SIZE * N);
		pthread_mutex_init(&cleaner_mutex, nullptr);
		pthread_cond_init(&cleaner_cond, nullptr);
		rooms.resize(N);
		cleaners.resize(X);
	}

	~Hotel()
	{
		// join all cleaners
		for (size_t i = 0; i < cleaners.size(); ++i)
		{
			pthread_join(cleaners[i], nullptr);
		}
		sem_destroy(&requestLeft);
	}

	void startCleaners()
	{
		for (int32_t i = 0; i < numCleaners; ++i)
		{
			auto ptr = new int32_t{i};
			pthread_create(&cleaners[i], nullptr, cleanerThread, ptr);
		}
	}

	bool allotRoom(int32_t guest, int32_t priority)
	{
		size_t most_suitable;
		int32_t min_priority = INT32_MAX;

		for (size_t i = 0; i < rooms.size(); ++i)
		{
			pthread_mutex_lock(&rooms[i].roomMutex);
			// printf("Room %ld is being checked for alloting\n", i);
			if (rooms[i].occupancy < ROOM_SIZE)
			{
				if (rooms[i].guestPriority < min_priority)
				{
					min_priority = rooms[i].guestPriority;
					most_suitable = i;
				}
			}
			pthread_mutex_unlock(&rooms[i].roomMutex);
		}

		// return false;
		printf("Most suitable room is for Guest %d is %lu\n", guest, most_suitable);
		bool alloted = rooms[most_suitable].allotGuest(guest, priority);
		if (alloted && rooms[most_suitable].occupancy == ROOM_SIZE)
		{
			printf("Room %lu just got dirty\n", most_suitable);
		}
		return alloted;
	}

	// can speed up both these functions
	void checkout(int32_t gid, int32_t time)
	{
		for (size_t i = 0; i < rooms.size(); ++i)
		{
			/*check if guest assigned this room*/
			if (rooms[i].guest == gid)
			{
				rooms[i].checkoutGuest(time);
				return;
			}
		}
	}
	bool checkGuestInHotel(int32_t gid)
	{
		for (size_t i = 0; i < rooms.size(); ++i)
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
