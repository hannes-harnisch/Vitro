#pragma once

namespace Vitro
{
	// Creates a bitmask with only the nth bit (from the right) turned on.
	constexpr int TurnOnBit(int n)
	{
		return 1 << n;
	}
}
