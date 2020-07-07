#pragma once

#include "_pch.h"
#include "Vitro/Client/Event.h"
#include "Vitro/Client/MouseCode.h"

namespace Vitro
{
	// Superclass for all mouse events.
	class MouseEvent : public Event
	{
	public:
		VTR_EVENT_SOURCE(EventSource::Input | EventSource::MouseButton);

		std::string ToString() const override
		{
			std::stringstream s;
			s << GetName() << ": " << Vitro::ToString(Button);
			return s.str();
		}

		MouseCode GetButton() const
		{
			return Button;
		}

	protected:
		MouseCode Button;

		MouseEvent(MouseCode button) : Button(button)
		{}
	};

	// Event fired when moving the mouse.
	class MouseMoveEvent final : public MouseEvent
	{
	public:
		VTR_EVENT_SOURCE(EventSource::Input | EventSource::Mouse);
		VTR_EVENT_TYPE(MouseMove);

		// Engine-internal constructor. Do NOT use in client application!
		MouseMoveEvent(int xDir, int yDir) : MouseEvent(MouseCode::None), XDirection(xDir), YDirection(yDir)
		{}

		std::string ToString() const override
		{
			std::stringstream s;
			s << GetName() << ": " << XDirection << ", " << YDirection;
			return s.str();
		}

		int GetXDirection() const
		{
			return XDirection;
		}

		int GetYDirection() const
		{
			return YDirection;
		}

	private:
		int XDirection, YDirection;
	};

	// Event fired when pressing down a mouse button.
	class MouseDownEvent final : public MouseEvent
	{
	public:
		VTR_EVENT_TYPE(MouseDown);

		// Engine-internal constructor. Do NOT use in client application!
		MouseDownEvent(MouseCode button) : MouseEvent(button)
		{}
	};

	// Event fired when releasing a mouse button.
	class MouseUpEvent final : public MouseEvent
	{
	public:
		VTR_EVENT_TYPE(MouseUp);

		// Engine-internal constructor. Do NOT use in client application!
		MouseUpEvent(MouseCode button) : MouseEvent(button)
		{}
	};

	// Event fired when double-clicking a mouse button.
	class DoubleClickEvent final : public MouseEvent
	{
	public:
		VTR_EVENT_TYPE(DoubleClick);

		// Engine-internal constructor. Do NOT use in client application!
		DoubleClickEvent(MouseCode button) : MouseEvent(button)
		{}
	};

	// Event fired when scrolling the mouse wheel.
	class MouseScrollEvent final : public MouseEvent
	{
	public:
		VTR_EVENT_SOURCE(EventSource::Input | EventSource::Mouse);
		VTR_EVENT_TYPE(MouseScroll);

		// Engine-internal constructor. Do NOT use in client application!
		MouseScrollEvent(float xOffset, float yOffset) :
			MouseEvent(MouseCode::Wheel), XOffset(xOffset), YOffset(yOffset)
		{}

		std::string ToString() const override
		{
			std::stringstream s;
			s << GetName() << ": " << XOffset << ", " << YOffset;
			return s.str();
		}

		// The fraction of the detent that was scrolled sideways.
		float GetXOffset() const
		{
			return XOffset;
		}

		// The fraction of the detent that was scrolled upwards.
		float GetYOffset() const
		{
			return YOffset;
		}

	private:
		float XOffset, YOffset;
	};
}
