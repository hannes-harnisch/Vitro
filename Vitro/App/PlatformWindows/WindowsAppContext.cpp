module;
#include "VitroCore/Macros.hpp"
#include "VitroCore/PlatformWindows/WindowsAPI.hpp"
module vt.App.Windows.AppContext;

import vt.App.AppContextBase;
import vt.App.EventSystem;
import vt.App.Window;
import vt.App.WindowEvent;
import vt.App.Windows.Window;
import vt.Core.LookupTable;
import vt.Core.Rect;
import vt.Core.SmallList;
import vt.Core.Vector;
import vt.Core.Windows.Utils;
import vt.Trace.Log;

namespace vt::windows
{
	constexpr inline auto VIRTUAL_KEY_LOOKUP = [] {
		LookupTable<WPARAM, KeyCode, UINT8_MAX> _;
		using enum KeyCode;

		_[VK_ESCAPE]			  = Escape;
		_[VK_F1]				  = F1;
		_[VK_F2]				  = F2;
		_[VK_F3]				  = F3;
		_[VK_F4]				  = F4;
		_[VK_F5]				  = F5;
		_[VK_F6]				  = F6;
		_[VK_F7]				  = F7;
		_[VK_F8]				  = F8;
		_[VK_F9]				  = F9;
		_[VK_F10]				  = F10;
		_[VK_F11]				  = F11;
		_[VK_F12]				  = F12;
		_[VK_F13]				  = F13;
		_[VK_F14]				  = F14;
		_[VK_F15]				  = F15;
		_[VK_F16]				  = F16;
		_[VK_F17]				  = F17;
		_[VK_F18]				  = F18;
		_[VK_F19]				  = F19;
		_[VK_F20]				  = F20;
		_[VK_F21]				  = F21;
		_[VK_F22]				  = F22;
		_[VK_F23]				  = F23;
		_[VK_F24]				  = F24;
		_[0x30]					  = Num0;
		_[0x31]					  = Num1;
		_[0x32]					  = Num2;
		_[0x33]					  = Num3;
		_[0x34]					  = Num4;
		_[0x35]					  = Num5;
		_[0x36]					  = Num6;
		_[0x37]					  = Num7;
		_[0x38]					  = Num8;
		_[0x39]					  = Num9;
		_[VK_BACK]				  = Backspace;
		_[VK_RETURN]			  = Enter;
		_[VK_TAB]				  = Tab;
		_[VK_CAPITAL]			  = CapsLock;
		_[VK_SHIFT]				  = Shift;
		_[VK_CONTROL]			  = Control;
		_[VK_LWIN]				  = SystemMenuLeft;
		_[VK_RWIN]				  = SystemMenuRight;
		_[VK_MENU]				  = Alt;
		_[VK_SPACE]				  = Space;
		_[VK_APPS]				  = Menu;
		_[0x41]					  = A;
		_[0x42]					  = B;
		_[0x43]					  = C;
		_[0x44]					  = D;
		_[0x45]					  = E;
		_[0x46]					  = F;
		_[0x47]					  = G;
		_[0x48]					  = H;
		_[0x49]					  = I;
		_[0x4A]					  = J;
		_[0x4B]					  = K;
		_[0x4C]					  = L;
		_[0x4D]					  = M;
		_[0x4E]					  = N;
		_[0x4F]					  = O;
		_[0x50]					  = P;
		_[0x51]					  = Q;
		_[0x52]					  = R;
		_[0x53]					  = S;
		_[0x54]					  = T;
		_[0x55]					  = U;
		_[0x56]					  = V;
		_[0x57]					  = W;
		_[0x58]					  = X;
		_[0x59]					  = Y;
		_[0x5A]					  = Z;
		_[VK_SNAPSHOT]			  = Print;
		_[VK_SCROLL]			  = ScrollLock;
		_[VK_PAUSE]				  = Pause;
		_[VK_INSERT]			  = Insert;
		_[VK_DELETE]			  = Delete;
		_[VK_HOME]				  = Home;
		_[VK_END]				  = End;
		_[VK_PRIOR]				  = PageUp;
		_[VK_NEXT]				  = PageDown;
		_[VK_LEFT]				  = ArrowLeft;
		_[VK_UP]				  = ArrowUp;
		_[VK_RIGHT]				  = ArrowRight;
		_[VK_DOWN]				  = ArrowDown;
		_[VK_CLEAR]				  = Clear;
		_[VK_DECIMAL]			  = Decimal;
		_[VK_ADD]				  = Add;
		_[VK_SUBTRACT]			  = Subtract;
		_[VK_MULTIPLY]			  = Multiply;
		_[VK_DIVIDE]			  = Divide;
		_[VK_NUMLOCK]			  = NumLock;
		_[VK_NUMPAD0]			  = Pad0;
		_[VK_NUMPAD1]			  = Pad1;
		_[VK_NUMPAD2]			  = Pad2;
		_[VK_NUMPAD3]			  = Pad3;
		_[VK_NUMPAD4]			  = Pad4;
		_[VK_NUMPAD5]			  = Pad5;
		_[VK_NUMPAD6]			  = Pad6;
		_[VK_NUMPAD7]			  = Pad7;
		_[VK_NUMPAD8]			  = Pad8;
		_[VK_NUMPAD9]			  = Pad9;
		_[VK_OEM_102]			  = AngleBracket;
		_[VK_OEM_PLUS]			  = Plus;
		_[VK_OEM_COMMA]			  = Comma;
		_[VK_OEM_MINUS]			  = Minus;
		_[VK_OEM_PERIOD]		  = Period;
		_[VK_OEM_1]				  = OEM1;
		_[VK_OEM_2]				  = OEM2;
		_[VK_OEM_3]				  = OEM3;
		_[VK_OEM_4]				  = OEM4;
		_[VK_OEM_5]				  = OEM5;
		_[VK_OEM_6]				  = OEM6;
		_[VK_OEM_7]				  = OEM7;
		_[VK_OEM_8]				  = OEM8;
		_[VK_BROWSER_BACK]		  = WebBack;
		_[VK_BROWSER_FORWARD]	  = WebForward;
		_[VK_BROWSER_REFRESH]	  = WebRefresh;
		_[VK_BROWSER_STOP]		  = WebStop;
		_[VK_BROWSER_SEARCH]	  = WebSearch;
		_[VK_BROWSER_FAVORITES]	  = WebFavorites;
		_[VK_BROWSER_HOME]		  = WebHome;
		_[VK_VOLUME_MUTE]		  = VolumeMute;
		_[VK_VOLUME_DOWN]		  = VolumeDown;
		_[VK_VOLUME_UP]			  = VolumeUp;
		_[VK_MEDIA_NEXT_TRACK]	  = MediaNext;
		_[VK_MEDIA_PREV_TRACK]	  = MediaPrevious;
		_[VK_MEDIA_STOP]		  = MediaStop;
		_[VK_MEDIA_PLAY_PAUSE]	  = MediaPlayPause;
		_[VK_LAUNCH_MAIL]		  = Mail;
		_[VK_LAUNCH_MEDIA_SELECT] = MediaSelect;
		_[VK_LAUNCH_APP1]		  = LaunchApp1;
		_[VK_LAUNCH_APP2]		  = LaunchApp2;
		return _;
	}();

	HMODULE query_module_handle()
	{
		auto handle = ::GetModuleHandle(nullptr);
		check_winapi_error(handle, "Failed to get module handle.");
		return handle;
	}

	KeyCode convert_virtual_key(WPARAM virtual_key)
	{
		auto key = VIRTUAL_KEY_LOOKUP[virtual_key];

		if(key == KeyCode::None)
			Log().error("Encountered unknown Windows virtual key code: ", virtual_key);

		return key;
	}

	MouseCode convert_extra_button(WPARAM w_param)
	{
		switch(HIWORD(w_param))
		{
			case XBUTTON1: return MouseCode::Extra1;
			case XBUTTON2: return MouseCode::Extra2;
		}
		VT_UNREACHABLE();
	}

	WindowsAppContext::WindowsAppContext() : AppContextBase(query_module_handle())
	{
		WNDCLASS const window_class {
			.style		   = CS_DBLCLKS,
			.lpfnWndProc   = forward_messages,
			.cbClsExtra	   = 0,
			.cbWndExtra	   = 0,
			.hInstance	   = get_system_window_owner(),
			.hIcon		   = nullptr,
			.hCursor	   = nullptr,
			.hbrBackground = nullptr,
			.lpszMenuName  = nullptr,
			.lpszClassName = WindowsWindow::WINDOW_CLASS_NAME,
		};
		auto atom = ::RegisterClass(&window_class);
		check_winapi_error(atom, "Failed to register window class.");

		RAWINPUTDEVICE const raw_input_device {
			.usUsagePage = 0x01, // Usage page constant for generic desktop controls
			.usUsage	 = 0x02, // Usage constant for a generic mouse
			.dwFlags	 = 0,
			.hwndTarget	 = nullptr,
		};
		auto succeeded = ::RegisterRawInputDevices(&raw_input_device, 1, sizeof(RAWINPUTDEVICE));
		check_winapi_error(succeeded, "Failed to register raw input device.");
	}

	void WindowsAppContext::pump_system_events()
	{
		MSG message;
		while(::PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}

	LRESULT CALLBACK WindowsAppContext::forward_messages(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
	{
		switch(message)
		{ // clang-format off
				case WM_MOVE:		   get().on_window_move(hwnd, l_param); return 0;
				case WM_SIZE:		   get().on_window_size(hwnd, l_param); return 0;
				case WM_ACTIVATE:	   get().restore_window_cursor_state(hwnd, w_param); return 0;
				case WM_SETFOCUS:	   get().on_window_event<WindowFocusEvent>(hwnd); return 0;
				case WM_KILLFOCUS:	   get().on_window_event<WindowUnfocusEvent>(hwnd); return 0;
				case WM_CLOSE:		   get().on_window_close(hwnd); return 0;
				case WM_SHOWWINDOW:	   get().on_window_show(hwnd, w_param); return 0;
				case WM_INPUT:		   get().on_raw_input(hwnd, l_param); return 0;
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:	   get().on_key_down(hwnd, w_param); return 0;
				case WM_KEYUP:
				case WM_SYSKEYUP:	   get().on_key_up(hwnd, w_param); return 0;
				case WM_CHAR:
				case WM_SYSCHAR:	   get().on_key_text(hwnd, w_param); return 0;
				case WM_UNICHAR:	   get().on_key_text(hwnd, w_param); return w_param == UNICODE_NOCHAR;
				case WM_MOUSEMOVE:	   get().store_last_mouse_position(l_param); return 0;
				case WM_LBUTTONDOWN:   get().on_mouse_event<MouseDownEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONDOWN:   get().on_mouse_event<MouseDownEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONDOWN:   get().on_mouse_event<MouseDownEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONDOWN:   get().on_mouse_event<MouseDownEvent>(hwnd, convert_extra_button(w_param)); return true;
				case WM_LBUTTONUP:	   get().on_mouse_event<MouseUpEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONUP:	   get().on_mouse_event<MouseUpEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONUP:	   get().on_mouse_event<MouseUpEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONUP:	   get().on_mouse_event<MouseUpEvent>(hwnd, convert_extra_button(w_param)); return true;
				case WM_LBUTTONDBLCLK: get().on_mouse_event<DoubleClickEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONDBLCLK: get().on_mouse_event<DoubleClickEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONDBLCLK: get().on_mouse_event<DoubleClickEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONDBLCLK: get().on_mouse_event<DoubleClickEvent>(hwnd, convert_extra_button(w_param)); return true;
				case WM_MOUSEWHEEL:	   get().on_vertical_scroll(hwnd, w_param); return 0;
				case WM_MOUSEHWHEEL:   get().on_horizontal_scroll(hwnd, w_param); return 0;
			} // clang-format on
		return ::DefWindowProc(hwnd, message, w_param, l_param);
	}

	void WindowsAppContext::on_window_move(HWND hwnd, LPARAM l_param) const
	{
		auto window = find_window(hwnd);
		if(!window)
			return;

		Int2 position {
			.x = GET_X_LPARAM(l_param),
			.y = GET_Y_LPARAM(l_param),
		};
		EventSystem::notify<WindowMoveEvent>(*window, position);
	}

	void WindowsAppContext::on_window_size(HWND hwnd, LPARAM l_param) const
	{
		Extent size {
			.width	= LOWORD(l_param),
			.height = HIWORD(l_param),
		};
		if(size.zero())
			return;

		auto window = find_window(hwnd);
		if(window)
			EventSystem::notify<WindowSizeEvent>(*window, size);
	}

	void WindowsAppContext::restore_window_cursor_state(HWND hwnd, WPARAM w_param) const
	{
		auto window = find_window(hwnd);
		if(!window || window->cursor_enabled())
			return;

		if(w_param & WA_ACTIVE || w_param & WA_CLICKACTIVE)
			window->disable_cursor();
	}

	template<typename E> void WindowsAppContext::on_window_event(HWND hwnd) const
	{
		auto window = find_window(hwnd);
		if(window)
			EventSystem::notify<E>(*window);
	}

	void WindowsAppContext::on_window_close(HWND hwnd) const
	{
		auto window = find_window(hwnd);
		if(window)
			window->close();
	}

	void WindowsAppContext::on_window_show(HWND hwnd, WPARAM w_param) const
	{
		if(w_param)
			on_window_event<WindowOpenEvent>(hwnd);
		else
			on_window_event<WindowCloseEvent>(hwnd);
	}

	void WindowsAppContext::on_raw_input(HWND hwnd, LPARAM l_param) const
	{
		auto window = find_window(hwnd);
		if(!window)
			return;

		auto input_handle = reinterpret_cast<HRAWINPUT>(l_param);
		UINT size;

		UINT result = ::GetRawInputData(input_handle, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
		VT_ENSURE(result != -1, "Failed to query raw input data size.");

		SmallList<BYTE> bytes(size);
		result = ::GetRawInputData(input_handle, RID_INPUT, bytes.data(), &size, sizeof(RAWINPUTHEADER));
		VT_ENSURE(result != -1, "Failed to read raw input data.");

		auto input = new(bytes.data()) RAWINPUT;
		Int2 direction {
			.x = input->data.mouse.lLastX,
			.y = -input->data.mouse.lLastY, // Negation results in more intuitive behavior.
		};
		EventSystem::notify<MouseMoveEvent>(*window, last_mouse_position, direction);
	}

	void WindowsAppContext::on_key_down(HWND hwnd, WPARAM w_param)
	{
		auto key = convert_virtual_key(w_param);

		if(key == last_key_code)
			++key_repeats;
		else
			key_repeats = 0;
		last_key_code = key;

		auto window = find_window(hwnd);
		if(window)
			EventSystem::notify<KeyDownEvent>(*window, key, key_repeats);
	}

	void WindowsAppContext::on_key_up(HWND hwnd, WPARAM w_param)
	{
		key_repeats	  = 0;
		last_key_code = KeyCode::None;

		auto window = find_window(hwnd);
		if(!window)
			return;

		auto key = convert_virtual_key(w_param);
		EventSystem::notify<KeyUpEvent>(*window, key);
	}

	void WindowsAppContext::on_key_text(HWND hwnd, WPARAM w_param) const
	{
		auto window = find_window(hwnd);
		if(!window)
			return;

		TCHAR const chars[] {static_cast<TCHAR>(w_param), TEXT('\0')};
		EventSystem::notify<KeyTextEvent>(*window, last_key_code, narrow_string(chars));
	}

	void WindowsAppContext::store_last_mouse_position(LPARAM l_param)
	{
		last_mouse_position.x = GET_X_LPARAM(l_param);
		last_mouse_position.y = GET_Y_LPARAM(l_param);
	}

	template<typename E> void WindowsAppContext::on_mouse_event(HWND hwnd, MouseCode button) const
	{
		auto window = find_window(hwnd);
		if(window)
			EventSystem::notify<E>(*window, button);
	}

	void WindowsAppContext::on_vertical_scroll(HWND hwnd, WPARAM w_param) const
	{
		auto window = find_window(hwnd);
		if(!window)
			return;

		Float2 offset {
			.y = GET_WHEEL_DELTA_WPARAM(w_param) / float(WHEEL_DELTA),
		};
		EventSystem::notify<MouseScrollEvent>(*window, offset);
	}

	void WindowsAppContext::on_horizontal_scroll(HWND hwnd, WPARAM w_param) const
	{
		auto window = find_window(hwnd);
		if(!window)
			return;

		Float2 offset {
			.x = GET_WHEEL_DELTA_WPARAM(w_param) / float(WHEEL_DELTA),
		};
		EventSystem::notify<MouseScrollEvent>(*window, offset);
	}
}
