#pragma once

#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include "Node.hpp"
#include "Action.hpp"
#include "Queue.hpp"
#include "Graph.hpp"
#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

struct Node;
extern vector<Node> nodes;
// extern queue<Action> shared_queue;
extern Queue<Action, 25> shared_queue;
// constexpr size_t MAX_QUEUE_SIZE = 100; // Use this to limit the size of the shared queue

/*declare a mutex and a condition variable*/
// extern pthread_mutex_t shared_queue_mutex;
// extern pthread_cond_t shared_queue_cond;
// extern pthread_mutex_t print_mutex; 
extern FILE *fptr;
void *userSimulatorRunner(void *);
void *pushUpdateRunner(void *);
void *readPostRunner(void *);
char *get_time(time_t);

#endif // _COMMON_HPP_

// extern used to declare a variable global that will be used in other files
