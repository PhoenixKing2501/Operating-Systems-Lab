#pragma once

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include "Action.hpp"
#include "Common.hpp"
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
				else if (this->sort_order == Type::Priority)
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

				return false;
			}};
	vector<size_t> *neighbors{nullptr};
	size_t past_actions[3]{};

	/*include parameters like feedQueue_mutex and feedQueue_cond*/
	pthread_mutex_t feedQueue_mutex;
	// pthread_cond_t feedQueue_cond;
	Node() = default;
	Node(size_t id, vector<size_t> *nebr)
		: id(id), past_actions{}, neighbors{nebr}
	{
		static mt19937_64 rng{random_device{}()};
		static uniform_int_distribution<int> dist{0, 1};
		this->sort_order = static_cast<Type>(dist(rng));
		this->feedQueue_mutex = PTHREAD_MUTEX_INITIALIZER;
		// this->feedQueue_cond = PTHREAD_COND_INITIALIZER;
	}

	Node(const Node &other) = default;
	Node(Node &&other) noexcept = default;
	Node &operator=(const Node &other) = default;
	Node &operator=(Node &&other) noexcept = default;
	~Node() = default;

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

	Action genNextAction()
	{
		static mt19937_64 rng{random_device{}()};
		static uniform_int_distribution<int> dist{0, 2};

		int action_type = dist(rng);

		this->past_actions[action_type]++;

		return Action{this->id,
					  this->past_actions[action_type],
					  static_cast<Action::Type>(action_type)};
	}
};

#endif // _NODE_HPP_
