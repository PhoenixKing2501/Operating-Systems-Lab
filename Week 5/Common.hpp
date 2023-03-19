#pragma once

#ifndef _COMMON_HPP_
#define _COMMON_HPP_

constexpr int ROOM_SIZE{2};

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

#include "Hotel.hpp"
#include "Room.hpp"

using namespace std;

void *guestThread(void *arg);
void *cleanerThread(void *arg);

#endif // _COMMON_HPP_
