#pragma once

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include "Action.hpp"
#include <bits/stdc++.h>
using namespace std;

struct Node;
extern vector<Node> nodes;

struct Node
{
	size_t id{};
	enum struct Type
	{
		Priority,
		Chronology,
	} sort_order{};
	priority_queue<Action, vector<Action>,
				   function<bool(const Action &, const Action &)>>
		wallQueue{
			[](const Action &a, const Action &b)
			{ return a.timestamp < b.timestamp; }};

	priority_queue<Action, vector<Action>,
				   function<bool(const Action &, const Action &)>>
		feedQueue{
			[&](const Action &a, const Action &b)
			{
				if (this->sort_order == Type::Chronology)
				{
					return a.timestamp < b.timestamp;
				}
				else
				{
					const auto &na = *nodes[a.user_id].neighbors;
					const auto &nb = *nodes[b.user_id].neighbors;

					vector<size_t> inta{}, intb{};

					set_intersection(na.begin(), na.end(),
									 this->neighbors->begin(),
									 this->neighbors->end(),
									 back_inserter(inta));

					set_intersection(nb.begin(), nb.end(),
									 this->neighbors->begin(),
									 this->neighbors->end(),
									 back_inserter(intb));

					return inta.size() < intb.size();
				}
			}};
	vector<size_t> *neighbors{nullptr};

	Node() = default;
	Node(size_t id)
		: id(id)
	{
		static mt19937_64 rng{random_device{}()};
		static uniform_int_distribution<int> dist{0, 1};
		this->sort_order = static_cast<Type>(dist(rng));
	}

	Node(const Node &other) = default;
	Node(Node &&other) noexcept = default;
	Node &operator=(const Node &other) = default;
	Node &operator=(Node &&other) noexcept = default;
	~Node() = default;

	void setNeighbors(vector<size_t> &neighbors)
	{
		this->neighbors = &neighbors;
	}

	void pushToWall(const Action &action)
	{
		this->wallQueue.push(action);
	}

	void pushToFeed(const Action &action)
	{
		this->feedQueue.push(action);
	}

	Action popFromWall()
	{
		auto action = this->wallQueue.top();
		this->wallQueue.pop();
		return action;
	}

	Action popFromFeed()
	{
		auto action = this->feedQueue.top();
		this->feedQueue.pop();
		return action;
	}
};

#endif // _NODE_HPP_
