#pragma once

#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

#include "Node.hpp"
#include <bits/stdc++.h>
using namespace std;

template <typename T>
struct Graph
{
	vector<vector<size_t>> adjList{};
	size_t size{};

	Graph() = default;
	Graph(size_t sz) : adjList(sz), size(sz){};
	Graph(const Graph &other) = default;
	Graph(Graph &&other) noexcept = default;
	Graph &operator=(const Graph &other) = default;
	Graph &operator=(Graph &&other) noexcept = default;
	~Graph() = default;
	void addEdge(size_t node1, size_t node2)
	{
		this->adjList[node1].push_back(node2);
		this->adjList[node2].push_back(node1);
	}
	void shrinkToFit()
	{
		ranges::for_each(this->adjList,
						 [](auto &v)
						 { v.shrink_to_fit(); });
	}

	// Get the neighbors of a node
	vector<size_t> &operator[](size_t index) { return this->adjList[index]; }
	const vector<size_t> &operator[](size_t index) const { return this->adjList[index]; }

	// Get the size of the graph
	size_t getSize() const { return this->size; }

	// Get the adjacency list
	const vector<vector<size_t>> &getAdjList() const { return this->adjList; }
};

#endif // _GRAPH_HPP_
