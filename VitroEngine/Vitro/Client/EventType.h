#pragma once

namespace Vitro
{
	enum class EventType : unsigned char
	{
		None = 0, // Never sent by an event. Only for variable initialization.
		WindowMove,
		WindowSize,
		WindowFocus,
		WindowUnfocus,
		WindowClose,
		WindowOpen,
		KeyDown,
		KeyUp,
		KeyText,
		MouseMove,
		MouseDown,
		MouseUp,
		DoubleClick,
		MouseScroll
	};
}
