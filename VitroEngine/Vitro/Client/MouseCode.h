#pragma once

#include "_pch.h"

namespace Vitro
{
	enum class MouseCode : unsigned char
	{
		None   = 0, // Associated with mouse move events.
		Mouse1 = 1, // The primary mouse button, usually the left physical button.
		Mouse2 = 2, // The secondary mouse button, usually the right physical button.
		Wheel  = 3, // The mouse wheel or middle mouse button.
		Extra1 = 4, // The first additional mouse button.
		Extra2 = 5	// The second additional mouse button.
	};

	std::string ToString(MouseCode button);
}
