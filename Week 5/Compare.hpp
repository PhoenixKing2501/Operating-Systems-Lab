#pragma once

#ifndef _COMPARE_HPP_
#define _COMPARE_HPP_

#include "Room.hpp"

class Compare
{
public:
	bool operator()(pair<int32_t,Room> &below, pair<int32_t,Room> &above)
	{
		if (below.second.guestPriority > above.second.guestPriority)
		{
			return true;
		}
		else if (below.second.guestPriority == above.second.guestPriority && below.second.occupancy > above.second.occupancy)
		{
			return true;
		}

		return false;
	}
};

#endif