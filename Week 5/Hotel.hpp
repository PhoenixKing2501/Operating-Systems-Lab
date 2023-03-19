#pragma once

#ifndef _HOTEL_HPP_
#define _HOTEL_HPP_

#include "Common.hpp"

struct Hotel
{
	vector<Room> rooms{};
	vector<pthread_t> cleaners{};
	sem_t requestLeft;

	Hotel(size_t Y, size_t N)
		: rooms(N), cleaners(Y)
	{
		sem_init(&requestLeft, 0, ROOM_SIZE * N);

		for (size_t i = 0; i < Y; i++)
		{
			pthread_create(&cleaners[i], NULL, cleaner, NULL);
		}
	}

	~Hotel()
	{
		sem_destroy(&requestLeft);
	}

	void allotGuest(pthread_t guest, int32_t priority)
	{
		for (size_t i = 0; i < rooms.size(); i++)
		{
			if (rooms[i].allotGuest(guest, priority))
			{
				return;
			}
		}

		sem_wait(&requestLeft);
	}
};

#endif // _HOTEL_HPP_
