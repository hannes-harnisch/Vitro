#pragma once

#include "_pch.h"
#include "Vitro/Client/Event.h"
#include "Vitro/Client/KeyCode.h"

namespace Vitro
{
	// Superclass for all keyboard events.
	class KeyEvent : public Event
	{
	public:
		VTR_EVENT_SOURCE(EventSource::Input | EventSource::Keyboard);

		inline KeyCode GetKey() const
		{
			return Key;
		}

	protected:
		KeyCode Key;

		inline KeyEvent(KeyCode key) : Key(key)
		{}
	};

	// Event fired when pressing down a keyboard key.
	class KeyDownEvent final : public KeyEvent
	{
	public:
		VTR_EVENT_TYPE(KeyDown);

		// Engine-internal constructor. Do NOT use in client application!
		inline KeyDownEvent(KeyCode key, int repeats) : KeyEvent(key), Repeats(repeats)
		{}

		inline std::string ToString() const override
		{
			std::stringstream s;
			s << GetName() << ": " << Vitro::ToString(Key) << " (Repeats: " << Repeats << ')';
			return s.str();
		}

		inline int GetRepeats() const
		{
			return Repeats;
		}

	private:
		int Repeats;
	};

	// Event fired when releasing a keyboard key.
	class KeyUpEvent final : public KeyEvent
	{
	public:
		VTR_EVENT_TYPE(KeyUp);

		// Engine-internal constructor. Do NOT use in client application!
		inline KeyUpEvent(KeyCode key) : KeyEvent(key)
		{}

		inline std::string ToString() const override
		{
			std::stringstream s;
			s << GetName() << ": " << Vitro::ToString(Key);
			return s.str();
		}
	};

	// Event fired when receiving text character input from the keyboard.
	class KeyTextEvent final : public KeyEvent
	{
	public:
		VTR_EVENT_TYPE(KeyText);

		// Engine-internal constructor. Do NOT use in client application!
		inline KeyTextEvent(KeyCode key, const std::string& text) : KeyEvent(key), Text(text)
		{}

		inline std::string ToString() const override
		{
			std::stringstream s;
			s << GetName() << ": " << Text << " (Key: " << Vitro::ToString(Key) << ')';
			return s.str();
		}

		inline std::string GetText() const
		{
			return Text;
		}

	private:
		std::string Text;
	};
}
