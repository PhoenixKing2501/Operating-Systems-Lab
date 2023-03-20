#pragma once

#ifndef _COMMON_HPP_
#define _COMMON_HPP_

constexpr int ROOM_SIZE{2};

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <valarray>

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define REQTIME 2
#define STAYTIME 3

void *guestThread(void *arg);
void *cleanerThread(void *arg);
// int my_printf(const char *format, ...);

extern vector<int32_t> pr_guests;
extern vector<pthread_mutex_t> guest_mutex;
extern vector<pthread_cond_t> guest_cond;
extern int cleaner_ctr;
extern int32_t numRooms;
extern int32_t numGuests;
extern int32_t numCleaners;

#include "Room.hpp"
#include "Hotel.hpp"
extern Hotel *hotel;

#endif // _COMMON_HPP_
