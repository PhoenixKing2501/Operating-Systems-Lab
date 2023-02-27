#pragma once

#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

#include <cstdint>
#include <exception>
#include <sys/shm.h>

constexpr size_t VEC_LEN = 200;
constexpr size_t SIZE = 20;

template <typename T>
class Graph
{
public:
	Graph(key_t key, size_t size = SIZE);
	~Graph() = default;

	void init(const T &val = T{});
	void remove_shm();
	size_t getSize() const;
	void setSize(size_t size);
	size_t getDegree(size_t i) const;
	T &operator()(size_t i, size_t j);
	const T &operator()(size_t i, size_t j) const;

private:
	T *data{nullptr};
	size_t *size{};
	int shmId{};
};

#include "Graph.inl"

#endif // _GRAPH_HPP_
