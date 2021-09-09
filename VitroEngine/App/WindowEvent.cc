module;
#include <format>
export module vt.App.WindowEvent;

import vt.App.Event;
import vt.Core.Enum;
import vt.Core.Rectangle;
import vt.Core.Vector;

namespace vt
{
	export enum class KeyCode : unsigned char {
		None		   = 0,
		Backspace	   = 8,
		Tab			   = 9,
		Clear		   = 12,
		Enter		   = 13,
		Shift		   = 16,
		Control		   = 17,
		Alt			   = 18,
		Pause		   = 19,
		CapsLock	   = 20,
		Escape		   = 27,
		Space		   = 32,
		PageUp		   = 33,
		PageDown	   = 34,
		End			   = 35,
		Home		   = 36,
		ArrowLeft	   = 37,
		ArrowUp		   = 38,
		ArrowRight	   = 39,
		ArrowDown	   = 40,
		Select		   = 41,
		Print		   = 42,
		Execute		   = 43,
		PrintScreen	   = 44,
		Insert		   = 45,
		Delete		   = 46,
		Help		   = 47,
		Number0		   = 48,
		Number1		   = 49,
		Number2		   = 50,
		Number3		   = 51,
		Number4		   = 52,
		Number5		   = 53,
		Number6		   = 54,
		Number7		   = 55,
		Number8		   = 56,
		Number9		   = 57,
		A			   = 65,
		B			   = 66,
		C			   = 67,
		D			   = 68,
		E			   = 69,
		F			   = 70,
		G			   = 71,
		H			   = 72,
		I			   = 73,
		J			   = 74,
		K			   = 75,
		L			   = 76,
		M			   = 77,
		N			   = 78,
		O			   = 79,
		P			   = 80,
		Q			   = 81,
		R			   = 82,
		S			   = 83,
		T			   = 84,
		U			   = 85,
		V			   = 86,
		W			   = 87,
		X			   = 88,
		Y			   = 89,
		Z			   = 90,
		SuperLeft	   = 91,
		SuperRight	   = 92,
		Menu		   = 93,
		Sleep		   = 95,
		Pad0		   = 96,
		Pad1		   = 97,
		Pad2		   = 98,
		Pad3		   = 99,
		Pad4		   = 100,
		Pad5		   = 101,
		Pad6		   = 102,
		Pad7		   = 103,
		Pad8		   = 104,
		Pad9		   = 105,
		Multiply	   = 106,
		Add			   = 107,
		Separator	   = 108,
		Subtract	   = 109,
		Decimal		   = 110,
		Divide		   = 111,
		F1			   = 112,
		F2			   = 113,
		F3			   = 114,
		F4			   = 115,
		F5			   = 116,
		F6			   = 117,
		F7			   = 118,
		F8			   = 119,
		F9			   = 120,
		F10			   = 121,
		F11			   = 122,
		F12			   = 123,
		F13			   = 124,
		F14			   = 125,
		F15			   = 126,
		F16			   = 127,
		F17			   = 128,
		F18			   = 129,
		F19			   = 130,
		F20			   = 131,
		F21			   = 132,
		F22			   = 133,
		F23			   = 134,
		F24			   = 135,
		NumLock		   = 144,
		ScrollLock	   = 145,
		OEM26		   = 146,
		OEM27		   = 147,
		OEM28		   = 148,
		OEM29		   = 149,
		OEM30		   = 150,
		ShiftLeft	   = 160,
		ShiftRight	   = 161,
		ControlLeft	   = 162,
		ControlRight   = 163,
		MenuLeft	   = 164,
		MenuRight	   = 165,
		WebBack		   = 166,
		WebForward	   = 167,
		WebRefresh	   = 168,
		WebStop		   = 169,
		WebSearch	   = 170,
		WebFavorites   = 171,
		WebHome		   = 172,
		VolumeMute	   = 173,
		VolumeDown	   = 174,
		VolumeUp	   = 175,
		MediaNext	   = 176,
		MediaPrevious  = 177,
		MediaStop	   = 178,
		MediaPlayPause = 179,
		Mail		   = 180,
		MediaSelect	   = 181,
		LaunchApp1	   = 182,
		LaunchApp2	   = 183,
		OEM1		   = 186,
		Plus		   = 187,
		Comma		   = 188,
		Minus		   = 189,
		Period		   = 190,
		OEM2		   = 191,
		OEM3		   = 192,
		OEM4		   = 219,
		OEM5		   = 220,
		OEM6		   = 221,
		OEM7		   = 222,
		OEM8		   = 223,
		OEM9		   = 225,
		AngleBracket   = 226,
		OEM10		   = 227,
		OEM11		   = 228,
		Process		   = 229,
		OEM12		   = 230,
		Unicode		   = 231,
		OEM13		   = 233,
		OEM14		   = 234,
		OEM15		   = 235,
		OEM16		   = 236,
		OEM17		   = 237,
		OEM18		   = 238,
		OEM19		   = 239,
		OEM20		   = 240,
		OEM21		   = 241,
		OEM22		   = 242,
		OEM23		   = 243,
		OEM24		   = 244,
		OEM25		   = 245,
	};

	export enum class MouseCode : unsigned char {
		None   = 0,
		Mouse1 = 1,
		Mouse2 = 2,
		Wheel  = 3,
		Extra1 = 4,
		Extra2 = 5,
	};

	class WindowEvent : public Event
	{
	public:
		class Window& window;

		WindowEvent(Window& window) : window(window)
		{}

		std::string to_string() const override
		{
			return std::format("{}: Window({})", Event::to_string(), static_cast<void*>(&window));
		}
	};

	export class WindowOpenEvent final : public WindowEvent
	{
	public:
		using WindowEvent::WindowEvent;
	};

	export class WindowCloseEvent final : public WindowEvent
	{
	public:
		using WindowEvent::WindowEvent;
	};

	export class WindowPaintEvent final : public WindowEvent
	{
	public:
		using WindowEvent::WindowEvent;
	};

	export class WindowFocusEvent final : public WindowEvent
	{
	public:
		using WindowEvent::WindowEvent;
	};

	export class WindowUnfocusEvent final : public WindowEvent
	{
	public:
		using WindowEvent::WindowEvent;
	};

	export class WindowSizeEvent final : public WindowEvent
	{
	public:
		Extent const size;

		WindowSizeEvent(Window& window, Extent size) : WindowEvent(window), size(size)
		{}

		std::string to_string() const override
		{
			return std::format("{}, Size({})", WindowEvent::to_string(), size.to_string());
		}
	};

	export class WindowMoveEvent final : public WindowEvent
	{
	public:
		Int2 const position;

		WindowMoveEvent(Window& window, Int2 position) : WindowEvent(window), position(position)
		{}

		std::string to_string() const override
		{
			return std::format("{}, Position({})", WindowEvent::to_string(), position.to_string());
		}
	};

	class KeyEvent : public WindowEvent
	{
	public:
		KeyCode const key;

		KeyEvent(Window& window, KeyCode key) : WindowEvent(window), key(key)
		{}

		std::string to_string() const override
		{
			return std::format("{}, Key({})", WindowEvent::to_string(), enum_name(key));
		}
	};

	export class KeyDownEvent final : public KeyEvent
	{
	public:
		unsigned const repeats;

		KeyDownEvent(Window& window, KeyCode key, unsigned repeats) : KeyEvent(window, key), repeats(repeats)
		{}

		std::string to_string() const override
		{
			return std::format("{}, Repeats({})", KeyEvent::to_string(), repeats);
		}
	};

	export class KeyUpEvent final : public KeyEvent
	{
	public:
		using KeyEvent::KeyEvent;
	};

	export class KeyTextEvent final : public KeyEvent
	{
	public:
		std::string const text;

		KeyTextEvent(Window& window, KeyCode key, std::string text) : KeyEvent(window, key), text(std::move(text))
		{}

		std::string to_string() const override
		{
			return std::format("{}, Text({})", KeyEvent::to_string(), text);
		}
	};

	class MouseEvent : public WindowEvent
	{
	public:
		MouseCode const button;

		MouseEvent(Window& window, MouseCode button) : WindowEvent(window), button(button)
		{}

		std::string to_string() const override
		{
			return std::format("{}, Button({})", WindowEvent::to_string(), enum_name(button));
		}
	};

	export class MouseMoveEvent final : public MouseEvent
	{
	public:
		Int2 const position, direction;

		MouseMoveEvent(Window& window, Int2 position, Int2 direction) :
			MouseEvent(window, MouseCode::None), position(position), direction(direction)
		{}

		std::string to_string() const override
		{
			return std::format("{}, Position({}), Direction({})", MouseEvent::to_string(), position.to_string(),
							   direction.to_string());
		}
	};

	export class MouseDownEvent final : public MouseEvent
	{
	public:
		using MouseEvent::MouseEvent;
	};

	export class MouseUpEvent final : public MouseEvent
	{
	public:
		using MouseEvent::MouseEvent;
	};

	export class DoubleClickEvent final : public MouseEvent
	{
	public:
		using MouseEvent::MouseEvent;
	};

	export class MouseScrollEvent final : public MouseEvent
	{
	public:
		Float2 const offset;

		MouseScrollEvent(Window& window, Float2 offset) : MouseEvent(window, MouseCode::Wheel), offset(offset)
		{}

		std::string to_string() const override
		{
			return std::format("{}, Offset({})", MouseEvent::to_string(), offset.to_string());
		}
	};
}
