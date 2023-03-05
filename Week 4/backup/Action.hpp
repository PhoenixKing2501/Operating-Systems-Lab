#pragma once

#ifndef _ACTION_HPP_
#define _ACTION_HPP_

#include <bits/stdc++.h>
using namespace std;

struct Action
{
	size_t user_id{};
	size_t action_id{};
	enum struct Type
	{
		Post,
		Comment,
		Like,
	} type{};
	time_t timestamp{};

	Action() = default;
	Action(size_t user_id, size_t action_id, Type type)
		: user_id(user_id), action_id(action_id), type(type), timestamp(time(NULL)){};
	Action(const Action &other) = default;
	Action(Action &&other) noexcept = default;
	Action &operator=(const Action &other) = default;
	Action &operator=(Action &&other) noexcept = default;
	~Action() = default;
};

#endif // _ACTION_HPP_
