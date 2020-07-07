#pragma once

#include "Vitro/Client/Event.h"
#include "Vitro/Client/Window.h"

namespace Vitro
{
	// Superclass for all window events.
	class WindowEvent : public Event
	{
	public:
		VTR_EVENT_SOURCE(EventSource::App);

		std::string ToString() const override
		{
			std::stringstream s;
			s << GetName() << ": " << &WindowRef;
			return s.str();
		}

		Window& GetWindow()
		{
			return WindowRef;
		}

	protected:
		Window& WindowRef;

		WindowEvent(Window& window) : WindowRef(window)
		{}
	};

	// Event fired when closing a window.
	class WindowCloseEvent final : public WindowEvent
	{
	public:
		VTR_EVENT_TYPE(WindowClose);

		// Engine-internal constructor. Do NOT use in client application!
		WindowCloseEvent(Window& window) : WindowEvent(window)
		{}
	};

	// Event fired when resizing a window.
	class WindowMoveEvent final : public WindowEvent
	{
	public:
		VTR_EVENT_TYPE(WindowMove);

		// Engine-internal constructor. Do NOT use in client application!
		WindowMoveEvent(Window& window, int x, int y) : WindowEvent(window), X(x), Y(y)
		{}

		std::string ToString() const override
		{
			std::stringstream s;
			s << GetName() << ": " << &WindowRef << ", " << X << ", " << Y;
			return s.str();
		}

		int GetX() const
		{
			return X;
		}

		int GetY() const
		{
			return Y;
		}

	private:
		int X, Y;
	};

	// Event fired when resizing a window.
	class WindowSizeEvent final : public WindowEvent
	{
	public:
		VTR_EVENT_TYPE(WindowSize);

		// Engine-internal constructor. Do NOT use in client application!
		WindowSizeEvent(Window& window, int width, int height) : WindowEvent(window), Width(width), Height(height)
		{}

		std::string ToString() const override
		{
			std::stringstream s;
			s << GetName() << ": " << &WindowRef << ", " << Width << ", " << Height;
			return s.str();
		}

		int GetWidth() const
		{
			return Width;
		}

		int GetHeight() const
		{
			return Height;
		}

	private:
		int Width, Height;
	};

	// Event fired when a window gets focus.
	class WindowFocusEvent final : public WindowEvent
	{
	public:
		VTR_EVENT_TYPE(WindowFocus);

		// Engine-internal constructor. Do NOT use in client application!
		WindowFocusEvent(Window& window) : WindowEvent(window)
		{}
	};

	// Event fired when a window loses focus.
	class WindowUnfocusEvent final : public WindowEvent
	{
	public:
		VTR_EVENT_TYPE(WindowUnfocus);

		// Engine-internal constructor. Do NOT use in client application!
		WindowUnfocusEvent(Window& window) : WindowEvent(window)
		{}
	};

	// Event fired when opening a window.
	class WindowOpenEvent final : public WindowEvent
	{
	public:
		VTR_EVENT_TYPE(WindowOpen);

		// Engine-internal constructor. Do NOT use in client application!
		WindowOpenEvent(Window& window) : WindowEvent(window)
		{}
	};
}
