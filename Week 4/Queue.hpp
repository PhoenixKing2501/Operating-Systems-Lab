#pragma once

#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

template <typename T, size_t N>
struct Queue
{
	array<queue<T, list<T>>, N> queues{};
	array<pthread_mutex_t, N> mutexes{};
	array<pthread_cond_t, N> conditionals{};
	pthread_mutex_t push_mutex = PTHREAD_MUTEX_INITIALIZER;
	size_t counter;

	Queue() : queues{}, counter{0}
	{
		for (auto &mutex : mutexes)
			mutex = PTHREAD_MUTEX_INITIALIZER;

		for (auto &conditional : conditionals)
			conditional = PTHREAD_COND_INITIALIZER;
	}
	Queue(const Queue &) = delete;
	Queue(Queue &&) = delete;
	Queue &operator=(const Queue &) = delete;
	Queue &operator=(Queue &&) = delete;
	~Queue() = default;

	void push(const T &value)
	{
		pthread_mutex_lock(&push_mutex);
		size_t index = counter++ % N;
		pthread_mutex_unlock(&push_mutex);

		pthread_mutex_lock(&mutexes[index]);
		queues[index].push(value);
		pthread_mutex_unlock(&mutexes[index]);
		pthread_cond_signal(&conditionals[index]);
	}

	T pop(size_t index)
	{
		pthread_mutex_lock(&mutexes[index]);
		while (queues[index].empty())
		{
			pthread_cond_wait(&conditionals[index], &mutexes[index]);
		}

		T value = queues[index].front();
		queues[index].pop();
		pthread_mutex_unlock(&mutexes[index]);
		return value;
	}
};

#endif // _QUEUE_HPP_
