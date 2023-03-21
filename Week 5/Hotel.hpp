#pragma once

#ifndef _HOTEL_HPP_
#define _HOTEL_HPP_

#include "Common.hpp"

struct Hotel
{
	vector<Room> rooms{};
	// construct a priority queue of rooms where priority is first based on guestPriority and then by occupancy wuth a custom comparator

	// priority_queue<Room,vector<Room>,cmprtr> guestPriorityQueue{};

	vector<pthread_t> cleaners{};
	sem_t requestLeft;
	pthread_cond_t cleaner_cond;
	pthread_mutex_t cleaner_mutex;

	Hotel(int32_t X, int32_t N)
	{
		sem_init(&requestLeft, 0, ROOM_SIZE * N);
		pthread_mutex_init(&cleaner_mutex, nullptr);
		pthread_cond_init(&cleaner_cond, nullptr);
		// number of rooms = N
		// so put priority queue of rooms in the hotel
		rooms.resize(N);
		// priority_queue<Room, rooms_container, Compare> rooms{};
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

	int allotRoom(int32_t guest, int32_t priority)
	{
		size_t most_suitable = numRooms;
		int32_t min_priority = INT32_MAX;

		for (size_t i = 0; i < rooms.size(); ++i)
		{
			pthread_mutex_lock(&rooms[i].roomMutex);
			if (rooms[i].guestPriority == -1 && rooms[i].occupancy < ROOM_SIZE)
			{
				min_priority = rooms[i].guestPriority;
				most_suitable = i;
				// pthread_mutex_unlock(&rooms[i].roomMutex);
				// break;
			}

			else if (rooms[i].guestPriority != -1 && rooms[i].occupancy < ROOM_SIZE - 1)
			{
				if (rooms[i].guestPriority < min_priority)
				{
					min_priority = rooms[i].guestPriority;
					most_suitable = i;
				}
			}
			pthread_mutex_unlock(&rooms[i].roomMutex);
		}

		printf("Most suitable room is for Guest %d is %lu\n", guest, most_suitable);
		bool alloted = rooms[most_suitable].allotGuest(guest, priority);

		return ((alloted) ? most_suitable : -1);
	}

	// can speed up both these functions
	void checkoutGuest(int32_t roomNumber, int32_t time)
	{
		rooms[roomNumber].checkoutGuest(time);
	}

	bool checkGuestInHotel(int32_t roomNumber, int32_t gid)
	{
		if (rooms[roomNumber].guest == gid)
		{
			return true;
		}
		return false;
	}

	void updateTotalTime(int32_t roomNumber, int32_t time)
	{
		rooms[roomNumber].updateTotalTime(time);
	}
};

#endif // _HOTEL_HPP_
