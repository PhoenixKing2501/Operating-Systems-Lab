#pragma once

#ifndef _COMMON_HPP_
#define _COMMON_HPP_

constexpr int ROOM_SIZE{2};

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "Hotel.hpp"
#include "Room.hpp"

using namespace std;

void *guestThread(void *arg);
void *cleanerThread(void *arg);

extern vector<int32_t> pr_guests;
extern Hotel *hotel;
extern vector<pthread_mutex_t> guest_mutex;
extern vector<pthread_cond_t> guest_cond;
extern int cleaner_ctr;
extern int32_t numRooms;
extern int32_t numGuests;
extern int32_t numCleaners;

#endif // _COMMON_HPP_
