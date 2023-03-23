#pragma once

#ifndef _HOTEL_HPP_
#define _HOTEL_HPP_

#include "Common.hpp"

struct Hotel
{
	vector<Room> rooms{};
	// construct a priority queue of rooms where priority is first based on guestPriority and then by occupancy wuth a custom comparator

	priority_queue<pair<int32_t, Room>, vector<pair<int32_t, Room>>, Compare> guestPriorityQueue{};

	vector<pthread_t> cleaners{};
	sem_t requestLeft;
	pthread_cond_t cleaner_cond;
	pthread_mutex_t cleaner_mutex;
	pthread_mutex_t guestPriorityQueue_mutex;

	Hotel(int32_t X, int32_t N)
	{
		sem_init(&requestLeft, 0, ROOM_SIZE * N);
		pthread_mutex_init(&cleaner_mutex, nullptr);
		pthread_mutex_init(&guestPriorityQueue_mutex, nullptr);
		pthread_cond_init(&cleaner_cond, nullptr);
		// number of rooms = N
		// so put priority queue of rooms in the hotel
		rooms.resize(N);
		// push N rooms with room number into the priority queue
		for (int32_t i = 0; i < N; ++i)
		{
			guestPriorityQueue.push(make_pair(i, rooms[i]));
		}
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
		int32_t most_suitable = numRooms;
		int32_t min_priority = INT32_MAX;

		// for (size_t i = 0; i < rooms.size(); ++i)
		// {
		// 	pthread_mutex_lock(&rooms[i].roomMutex);
		// 	if (rooms[i].guestPriority == -1 && rooms[i].occupancy < ROOM_SIZE)
		// 	{
		// 		min_priority = rooms[i].guestPriority;
		// 		most_suitable = i;
		// 		// pthread_mutex_unlock(&rooms[i].roomMutex);
		// 		// break;
		// 	}

		// 	else if (rooms[i].guestPriority != -1 && rooms[i].occupancy < ROOM_SIZE - 1)
		// 	{
		// 		if (rooms[i].guestPriority < min_priority)
		// 		{
		// 			min_priority = rooms[i].guestPriority;
		// 			most_suitable = i;
		// 		}
		// 	}
		// 	pthread_mutex_unlock(&rooms[i].roomMutex);
		// }
		// get the most suitable room from the priority queue and pop it
		pthread_mutex_lock(&guestPriorityQueue_mutex);
		most_suitable = guestPriorityQueue.top().first;
		guestPriorityQueue.pop();
		pthread_mutex_unlock(&guestPriorityQueue_mutex);

		printf("Most suitable room is for Guest %d is %d\n", guest, most_suitable);
		bool alloted = rooms[most_suitable].allotGuest(guest, priority);

		// push back to the priority queue
		pthread_mutex_lock(&guestPriorityQueue_mutex);
		guestPriorityQueue.push(make_pair(most_suitable, rooms[most_suitable]));
		pthread_mutex_unlock(&guestPriorityQueue_mutex);

		return ((alloted) ? most_suitable : -1);
	}

	// can speed up both these functions
	void checkoutGuest(int32_t roomNumber, int32_t time)
	{
		rooms[roomNumber].checkoutGuest(time);
		// find that element in priority queue with this room Number and update it
		priority_queue<pair<int32_t, Room>> temp{};
		pthread_mutex_lock(&guestPriorityQueue_mutex);
		while (!guestPriorityQueue.empty())
		{
			if (guestPriorityQueue.top().first == roomNumber)
			{
				guestPriorityQueue.pop();
				guestPriorityQueue.push(make_pair(roomNumber, rooms[roomNumber]));
				break;
			}
			temp.push(guestPriorityQueue.top());
			guestPriorityQueue.pop();
		}
		while (!temp.empty())
		{
			guestPriorityQueue.push(temp.top());
			temp.pop();
		}
		pthread_mutex_unlock(&guestPriorityQueue_mutex);
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
		// find that element in priority queue with this room Number and update it
		priority_queue<pair<int32_t, Room>> temp{};
		pthread_mutex_lock(&guestPriorityQueue_mutex);
		while (!guestPriorityQueue.empty())
		{
			if (guestPriorityQueue.top().first == roomNumber)
			{
				guestPriorityQueue.pop();
				guestPriorityQueue.push(make_pair(roomNumber, rooms[roomNumber]));
				break;
			}
			temp.push(guestPriorityQueue.top());
			guestPriorityQueue.pop();
		}
		while (!temp.empty())
		{
			guestPriorityQueue.push(temp.top());
			temp.pop();
		}
		pthread_mutex_unlock(&guestPriorityQueue_mutex);
	}
};

#endif // _HOTEL_HPP_
