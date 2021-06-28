module;
#include "Core/Enum.hh"

#include <format>
export module Vitro.App.WindowEvent;

import Vitro.App.Event;
import Vitro.App.KeyCode;
import Vitro.App.MouseCode;
import Vitro.App.Window;
import Vitro.Math.Rectangle;
import Vitro.Math.Vector;

namespace vt
{
	class WindowEvent : public Event
	{
	public:
		Window& window;

		std::string toString() const override
		{
			return std::format("{}: Window({})", Event::toString(), reinterpret_cast<uint64_t>(window.handle()));
		}

	protected:
		WindowEvent(Window& window) : window(window)
		{}
	};

	export class WindowOpenEvent final : public WindowEvent
	{
	public:
		WindowOpenEvent(Window& window) : WindowEvent(window)
		{}
	};

	export class WindowCloseEvent final : public WindowEvent
	{
	public:
		WindowCloseEvent(Window& window) : WindowEvent(window)
		{}
	};

	export class WindowFocusEvent final : public WindowEvent
	{
	public:
		WindowFocusEvent(Window& window) : WindowEvent(window)
		{}
	};

	export class WindowUnfocusEvent final : public WindowEvent
	{
	public:
		WindowUnfocusEvent(Window& window) : WindowEvent(window)
		{}
	};

	export class WindowSizeEvent final : public WindowEvent
	{
	public:
		Rectangle const size;

		WindowSizeEvent(Window& window, Rectangle size) : WindowEvent(window), size(size)
		{}

		std::string toString() const override
		{
			return std::format("{}, Size({})", WindowEvent::toString(), size.toString());
		}
	};

	export class WindowMoveEvent final : public WindowEvent
	{
	public:
		Int2 const position;

		WindowMoveEvent(Window& window, Int2 position) : WindowEvent(window), position(position)
		{}

		std::string toString() const override
		{
			return std::format("{}, Position({})", WindowEvent::toString(), position.toString());
		}
	};

	class KeyEvent : public WindowEvent
	{
	public:
		KeyCode const key;

		std::string toString() const override
		{
			return std::format("{}, Key({})", WindowEvent::toString(), enum_name(key));
		}

	protected:
		KeyEvent(Window& window, KeyCode key) : WindowEvent(window), key(key)
		{}
	};

	export class KeyDownEvent final : public KeyEvent
	{
	public:
		uint32_t const repeats;

		KeyDownEvent(Window& window, KeyCode key, uint32_t repeats) : KeyEvent(window, key), repeats(repeats)
		{}

		std::string toString() const override
		{
			return std::format("{}, Repeats({})", KeyEvent::toString(), repeats);
		}
	};

	export class KeyUpEvent final : public KeyEvent
	{
	public:
		KeyUpEvent(Window& window, KeyCode key) : KeyEvent(window, key)
		{}
	};

	export class KeyTextEvent final : public KeyEvent
	{
	public:
		std::string const text;

		KeyTextEvent(Window& window, KeyCode key, std::string text) : KeyEvent(window, key), text(std::move(text))
		{}

		std::string toString() const override
		{
			return std::format("{}, Text({})", KeyEvent::toString(), text);
		}
	};

	class MouseEvent : public WindowEvent
	{
	public:
		MouseCode const button;

		std::string toString() const override
		{
			return std::format("{}, Button({})", WindowEvent::toString(), enum_name(button));
		}

	protected:
		MouseEvent(Window& window, MouseCode button) : WindowEvent(window), button(button)
		{}
	};

	export class MouseMoveEvent final : public MouseEvent
	{
	public:
		Int2 const position, direction;

		MouseMoveEvent(Window& window, Int2 position, Int2 direction) :
			MouseEvent(window, MouseCode::None), position(position), direction(direction)
		{}

		std::string toString() const override
		{
			return std::format("{}, Position({}), Direction({})", MouseEvent::toString(), position.toString(),
							   direction.toString());
		}
	};

	export class MouseDownEvent final : public MouseEvent
	{
	public:
		MouseDownEvent(Window& window, MouseCode button) : MouseEvent(window, button)
		{}
	};

	export class MouseUpEvent final : public MouseEvent
	{
	public:
		MouseUpEvent(Window& window, MouseCode button) : MouseEvent(window, button)
		{}
	};

	export class DoubleClickEvent final : public MouseEvent
	{
	public:
		DoubleClickEvent(Window& window, MouseCode button) : MouseEvent(window, button)
		{}
	};

	export class MouseScrollEvent final : public MouseEvent
	{
	public:
		Float2 const offset;

		MouseScrollEvent(Window& window, Float2 offset) : MouseEvent(window, MouseCode::Wheel), offset(offset)
		{}

		std::string toString() const override
		{
			return std::format("{}, Offset({})", MouseEvent::toString(), offset.toString());
		}
	};
}
