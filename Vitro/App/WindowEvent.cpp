module;
#include <format>
export module vt.App.WindowEvent;

import vt.App.Window;
import vt.Core.Enum;
import vt.Core.Rect;
import vt.Core.Vector;

namespace vt
{
	export enum class KeyCode : uint8_t {
		None,
		Escape,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		Num0,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,
		Backspace,
		Enter,
		Tab,
		CapsLock,
		Shift,
		Control,
		SystemMenuLeft,
		SystemMenuRight,
		Alt,
		Space,
		Menu,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		Print,
		ScrollLock,
		Pause,
		Insert,
		Delete,
		Home,
		End,
		PageUp,
		PageDown,
		ArrowLeft,
		ArrowUp,
		ArrowRight,
		ArrowDown,
		Clear,
		Decimal,
		Add,
		Subtract,
		Multiply,
		Divide,
		NumLock,
		Pad0,
		Pad1,
		Pad2,
		Pad3,
		Pad4,
		Pad5,
		Pad6,
		Pad7,
		Pad8,
		Pad9,
		AngleBracket,
		Plus,
		Comma,
		Minus,
		Period,
		OEM1,
		OEM2,
		OEM3,
		OEM4,
		OEM5,
		OEM6,
		OEM7,
		OEM8,
		WebBack,
		WebForward,
		WebRefresh,
		WebStop,
		WebSearch,
		WebFavorites,
		WebHome,
		VolumeMute,
		VolumeDown,
		VolumeUp,
		MediaNext,
		MediaPrevious,
		MediaStop,
		MediaPlayPause,
		Mail,
		MediaSelect,
		LaunchApp1,
		LaunchApp2,
	};

	export enum class MouseCode : uint8_t {
		None,
		Mouse1,
		Mouse2,
		Wheel,
		Extra1,
		Extra2,
	};

	struct WindowEvent
	{
		Window& window;

		std::string to_string() const
		{
			auto handle = reinterpret_cast<uintptr_t>(window.native_handle());
			return std::format("Window({})", handle);
		}
	};

	export struct WindowOpenEvent : WindowEvent
	{};

	export struct WindowCloseEvent : WindowEvent
	{};

	export struct WindowFocusEvent : WindowEvent
	{};

	export struct WindowUnfocusEvent : WindowEvent
	{};

	export struct WindowSizeEvent : WindowEvent
	{
		Extent const size;

		std::string to_string() const
		{
			return std::format("{}, Size({})", WindowEvent::to_string(), size.to_string());
		}
	};

	export struct WindowMoveEvent : WindowEvent
	{
		Int2 const position;

		std::string to_string() const
		{
			return std::format("{}, Position({})", WindowEvent::to_string(), position.to_string());
		}
	};

	struct KeyEvent : WindowEvent
	{
		KeyCode const key;

		std::string to_string() const
		{
			return std::format("{}, Key({})", WindowEvent::to_string(), enum_name(key));
		}
	};

	export struct KeyDownEvent : KeyEvent
	{
		unsigned const repeats;

		std::string to_string() const
		{
			return std::format("{}, Repeats({})", KeyEvent::to_string(), repeats);
		}
	};

	export struct KeyUpEvent : KeyEvent
	{};

	export struct KeyTextEvent : KeyEvent
	{
		std::string const text;

		std::string to_string() const
		{
			return std::format("{}, Text({})", KeyEvent::to_string(), text);
		}
	};

	export struct MouseMoveEvent : WindowEvent
	{
		Int2 const position, direction;

		std::string to_string() const
		{
			return std::format("{}, Position({}), Direction({})", WindowEvent::to_string(), position.to_string(),
							   direction.to_string());
		}
	};

	export struct MouseScrollEvent : WindowEvent
	{
		Float2 const offset;

		std::string to_string() const
		{
			return std::format("{}, Offset({})", WindowEvent::to_string(), offset.to_string());
		}
	};

	struct MouseClickEvent : WindowEvent
	{
		MouseCode const button;

		std::string to_string() const
		{
			return std::format("{}, Button({})", WindowEvent::to_string(), enum_name(button));
		}
	};

	export struct MouseDownEvent : MouseClickEvent
	{};

	export struct MouseUpEvent : MouseClickEvent
	{};

	export struct DoubleClickEvent : MouseClickEvent
	{};
}
