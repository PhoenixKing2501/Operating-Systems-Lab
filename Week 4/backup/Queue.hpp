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
	pthread_mutex_t push_mutex = PTHREAD_MUTEX_INITIALIZER;
	static size_t counter;

	Queue()
		: queues{}, mutexes{}
	{
		for (auto &mutex : mutexes)
			mutex = PTHREAD_MUTEX_INITIALIZER;
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
	}

	void push(T &&value)
	{
		pthread_mutex_lock(&push_mutex);
		size_t index = counter++ % N;
		pthread_mutex_unlock(&push_mutex);

		pthread_mutex_lock(&mutexes[index]);
		queues[index].push(move(value));
		pthread_mutex_unlock(&mutexes[index]);
	}

	optional<T> pop(size_t index)
	{
		if (index >= N)
			return nullopt;
		pthread_mutex_lock(&mutexes[index]);
		if (queues[index].empty())
		{
			pthread_mutex_unlock(&mutexes[index]);
			return nullopt;
		}
		T value = queues[index].front();
		queues[index].pop();
		pthread_mutex_unlock(&mutexes[index]);
		return value;
	}
};

template <typename T, size_t N>
size_t Queue<T, N>::counter = 0;

#endif // _QUEUE_HPP_
