#pragma once

#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

#include <cstdint>
#include <exception>
#include <sys/shm.h>

constexpr size_t VEC_LEN = 5'000;

template <typename T>
class Graph
{
public:
	Graph(key_t key, size_t size = VEC_LEN);
	~Graph();

	void init(const T &val = T{});
	size_t getSize() const;
	T &operator()(size_t i, size_t j);
	const T &operator()(size_t i, size_t j) const;

private:
	T *m_data = nullptr;
	size_t m_size = 0;
	int shmId;
};

#include "Graph.inl"

#endif // _GRAPH_HPP_
