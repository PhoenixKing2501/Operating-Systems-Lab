#pragma once

#ifndef _HOTEL_HPP_
#define _HOTEL_HPP_

#include "Common.hpp"

struct Hotel
{
	vector<Room> rooms{};
	vector<pthread_t> cleaners{};
	sem_t requestLeft;
	sem_t CleanerSem;
	pthread_cond_t cleaner_cond;
	pthread_mutex_t cleaner_mutex;
	pthread_mutex_t roomAllot_mutex;
	int32_t roomToClean;
	int32_t roomsCleaned;

	Hotel(int32_t X, int32_t N)
	{
		sem_init(&requestLeft, 0, ROOM_SIZE * N);
		sem_init(&CleanerSem, 0, 0);
		pthread_mutex_init(&roomAllot_mutex, nullptr);
		pthread_mutex_init(&cleaner_mutex, nullptr);
		pthread_cond_init(&cleaner_cond, nullptr);
		roomToClean = -1;
		roomsCleaned = -1;
		rooms.resize(N);
		cleaners.resize(X);
	}

	~Hotel()
	{

		for (size_t i = 0; i < cleaners.size(); ++i)
		{
			pthread_join(cleaners[i], nullptr);
		}
		sem_destroy(&requestLeft);
		sem_destroy(&CleanerSem);
		pthread_mutex_destroy(&roomAllot_mutex);
		pthread_cond_destroy(&cleaner_cond);
		pthread_mutex_destroy(&cleaner_mutex);
	}
	void startCleaners()
	{
		roomToClean = -1;
		roomsCleaned = -1;
		for (int32_t i = 0; i < numCleaners; ++i)
		{
			auto ptr = new int32_t{i};
			pthread_create(&cleaners[i], nullptr, cleanerThread, ptr);
		}
	}

	int allotRoom(int32_t guest, int32_t priority)
	{
		int32_t most_suitable{numRooms};
		int32_t min_priority = INT32_MAX;

		pthread_mutex_lock(&roomAllot_mutex);

		for (size_t i = 0; i < rooms.size() && min_priority != -1; ++i)
		{

			if (rooms[i].occupancy < ROOM_SIZE)
			{
				if (rooms[i].guestPriority < min_priority)
				{
					min_priority = rooms[i].guestPriority;
					most_suitable = i;
				}
			}
		}

		printf("Most suitable room for Guest %d is %d\n", guest, most_suitable);
		bool alloted = rooms[most_suitable].allotGuest(guest, priority);

		pthread_mutex_unlock(&roomAllot_mutex);

		return ((alloted) ? most_suitable : -1);
	}

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

#endif