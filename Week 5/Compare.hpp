#pragma once

#ifndef _COMPARE_HPP_
#define _COMPARE_HPP_

#include "Common.hpp"

class Compare
{
public:
	bool operator()(Room &below, Room &above)
	{
		if (below.guestPriority > above.guestPriority)
		{
			return true;
		}
		else if (below.guestPriority == above.guestPriority && below.occupancy > above.occupancy)
		{
			return true;
		}

		return false;
	}
};

#endif