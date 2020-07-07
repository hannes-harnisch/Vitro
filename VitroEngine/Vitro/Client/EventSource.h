#pragma once

#include "_pch.h"
#include "Vitro/Math/Bit.h"

namespace Vitro
{
	// Indicates the device type or source associated with an event type.
	enum class EventSource : unsigned char
	{
		None		= 0, // Never sent by an event. Only for variable initialization.
		App			= TurnOnBit(0),
		Input		= TurnOnBit(1),
		Keyboard	= TurnOnBit(2),
		Mouse		= TurnOnBit(3),
		MouseButton = TurnOnBit(4)
	};

	// For using this enum as a bitmask.

	inline uint8_t operator&(EventSource s1, EventSource s2)
	{
		return static_cast<uint8_t>(s1) & static_cast<uint8_t>(s2);
	}

	inline EventSource operator|(EventSource s1, EventSource s2)
	{
		return static_cast<EventSource>(static_cast<uint8_t>(s1) | static_cast<uint8_t>(s2));
	}
}
