module;
#include "Core/Macros.hh"
#include "Windows.API.hh"

#include <any>
#include <new>
#include <unordered_map>
export module vt.Windows.AppContext;

import vt.App.EventSystem;
import vt.App.Window;
import vt.App.WindowEvent;
import vt.Core.Array;
import vt.Core.Rectangle;
import vt.Core.Singleton;
import vt.Core.Vector;
import vt.Trace.Log;
import vt.Windows.Utils;
import vt.Windows.Window;

namespace vt::windows
{
	KeyCode convert_virtual_key(WPARAM virtual_key)
	{
		using enum KeyCode;
		switch(virtual_key)
		{
			case VK_ESCAPE: return Escape;
			case VK_F1: return F1;
			case VK_F2: return F2;
			case VK_F3: return F3;
			case VK_F4: return F4;
			case VK_F5: return F5;
			case VK_F6: return F6;
			case VK_F7: return F7;
			case VK_F8: return F8;
			case VK_F9: return F9;
			case VK_F10: return F10;
			case VK_F11: return F11;
			case VK_F12: return F12;
			case VK_F13: return F13;
			case VK_F14: return F14;
			case VK_F15: return F15;
			case VK_F16: return F16;
			case VK_F17: return F17;
			case VK_F18: return F18;
			case VK_F19: return F19;
			case VK_F20: return F20;
			case VK_F21: return F21;
			case VK_F22: return F22;
			case VK_F23: return F23;
			case VK_F24: return F24;
			case 0x30: return Num0;
			case 0x31: return Num1;
			case 0x32: return Num2;
			case 0x33: return Num3;
			case 0x34: return Num4;
			case 0x35: return Num5;
			case 0x36: return Num6;
			case 0x37: return Num7;
			case 0x38: return Num8;
			case 0x39: return Num9;
			case VK_BACK: return Backspace;
			case VK_RETURN: return Enter;
			case VK_TAB: return Tab;
			case VK_CAPITAL: return CapsLock;
			case VK_SHIFT: return Shift;
			case VK_CONTROL: return Control;
			case VK_LWIN: return SystemMenuLeft;
			case VK_RWIN: return SystemMenuRight;
			case VK_MENU: return Alt;
			case VK_SPACE: return Space;
			case VK_APPS: return Menu;
			case 0x41: return A;
			case 0x42: return B;
			case 0x43: return C;
			case 0x44: return D;
			case 0x45: return E;
			case 0x46: return F;
			case 0x47: return G;
			case 0x48: return H;
			case 0x49: return I;
			case 0x4A: return J;
			case 0x4B: return K;
			case 0x4C: return L;
			case 0x4D: return M;
			case 0x4E: return N;
			case 0x4F: return O;
			case 0x50: return P;
			case 0x51: return Q;
			case 0x52: return R;
			case 0x53: return S;
			case 0x54: return T;
			case 0x55: return U;
			case 0x56: return V;
			case 0x57: return W;
			case 0x58: return X;
			case 0x59: return Y;
			case 0x5A: return Z;
			case VK_SNAPSHOT: return Print;
			case VK_SCROLL: return ScrollLock;
			case VK_PAUSE: return Pause;
			case VK_INSERT: return Insert;
			case VK_DELETE: return Delete;
			case VK_HOME: return Home;
			case VK_END: return End;
			case VK_PRIOR: return PageUp;
			case VK_NEXT: return PageDown;
			case VK_LEFT: return ArrowLeft;
			case VK_UP: return ArrowUp;
			case VK_RIGHT: return ArrowRight;
			case VK_DOWN: return ArrowDown;
			case VK_CLEAR: return Clear;
			case VK_DECIMAL: return Decimal;
			case VK_ADD: return Add;
			case VK_SUBTRACT: return Subtract;
			case VK_MULTIPLY: return Multiply;
			case VK_DIVIDE: return Divide;
			case VK_NUMLOCK: return NumLock;
			case VK_NUMPAD0: return Pad0;
			case VK_NUMPAD1: return Pad1;
			case VK_NUMPAD2: return Pad2;
			case VK_NUMPAD3: return Pad3;
			case VK_NUMPAD4: return Pad4;
			case VK_NUMPAD5: return Pad5;
			case VK_NUMPAD6: return Pad6;
			case VK_NUMPAD7: return Pad7;
			case VK_NUMPAD8: return Pad8;
			case VK_NUMPAD9: return Pad9;
		}
		Log().error("Encountered unknown Windows virtual key code: ", virtual_key);
		return None;
	}

	export class WindowsAppContext : public Singleton<WindowsAppContext>
	{
	protected:
		WindowsAppContext()
		{
			WNDCLASS const window_class {
				.style		   = CS_DBLCLKS,
				.lpfnWndProc   = forward_messages,
				.hInstance	   = ::GetModuleHandleW(nullptr),
				.lpszClassName = WindowsWindow::WindowClassName,
			};
			ATOM registered = ::RegisterClassW(&window_class);
			VT_ENSURE(registered, "Failed to register window class.");

			RAWINPUTDEVICE const raw_input_device {
				.usUsagePage = 0x01, // Usage page constant for generic desktop controls
				.usUsage	 = 0x02, // Usage constant for a generic mouse
			};
			BOOL succeeded = ::RegisterRawInputDevices(&raw_input_device, 1, sizeof(RAWINPUTDEVICE));
			VT_ENSURE(succeeded, "Failed to register raw input device.");
		}

		void poll_events() const
		{
			MSG message;
			while(::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&message);
				::DispatchMessageW(&message);
			}
		}

		std::unordered_map<void*, Window*>& get_native_window_handle_map()
		{
			return hwnd_to_vitro_window;
		}

	private:
		std::unordered_map<void*, Window*> hwnd_to_vitro_window;

		unsigned key_repeats		 = 0;
		KeyCode	 last_key_code		 = KeyCode::None;
		Int2	 last_mouse_position = {};

		static LRESULT CALLBACK forward_messages(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			switch(message)
			{
				case WM_MOVE: get().on_window_move(hwnd, l_param); return 0;
				case WM_SIZE: get().on_window_size(hwnd, l_param); return 0;
				case WM_ACTIVATE: get().restore_window_cursor_state(hwnd, w_param); return 0;
				case WM_SETFOCUS: get().on_window_event<WindowFocusEvent>(hwnd); return 0;
				case WM_KILLFOCUS: get().on_window_event<WindowUnfocusEvent>(hwnd); return 0;
				case WM_PAINT: get().on_window_event<WindowPaintEvent>(hwnd); return 0;
				case WM_CLOSE: get().on_window_close(hwnd); return 0;
				case WM_SHOWWINDOW: get().on_window_show(hwnd, w_param); return 0;
				case WM_INPUT: get().on_raw_input(hwnd, l_param); return 0;
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN: get().on_key_down(hwnd, w_param); return 0;
				case WM_KEYUP:
				case WM_SYSKEYUP: get().on_key_up(hwnd, w_param); return 0;
				case WM_CHAR:
				case WM_SYSCHAR: get().on_key_text(hwnd, w_param); return 0;
				case WM_UNICHAR: get().on_key_text(hwnd, w_param); return w_param == UNICODE_NOCHAR;
				case WM_MOUSEMOVE: get().store_last_mouse_position(l_param); return 0;
				case WM_LBUTTONDOWN: get().on_mouse_event<MouseDownEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONDOWN: get().on_mouse_event<MouseDownEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONDOWN: get().on_mouse_event<MouseDownEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONDOWN: get().on_mouse_event<MouseDownEvent>(hwnd, convert_extra_button(w_param)); return true;
				case WM_LBUTTONUP: get().on_mouse_event<MouseUpEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONUP: get().on_mouse_event<MouseUpEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONUP: get().on_mouse_event<MouseUpEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONUP: get().on_mouse_event<MouseUpEvent>(hwnd, convert_extra_button(w_param)); return true;
				case WM_LBUTTONDBLCLK: get().on_mouse_event<DoubleClickEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONDBLCLK: get().on_mouse_event<DoubleClickEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONDBLCLK: get().on_mouse_event<DoubleClickEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONDBLCLK: get().on_mouse_event<DoubleClickEvent>(hwnd, convert_extra_button(w_param)); return true;
				case WM_MOUSEWHEEL: get().on_vertical_scroll(hwnd, w_param); return 0;
				case WM_MOUSEHWHEEL: get().on_horizontal_scroll(hwnd, w_param); return 0;
			}
			return ::DefWindowProcW(hwnd, message, w_param, l_param);
		}

		static MouseCode convert_extra_button(WPARAM w_param)
		{
			switch(HIWORD(w_param))
			{
				case XBUTTON1: return MouseCode::Extra1;
				case XBUTTON2: return MouseCode::Extra2;
			}
			VT_UNREACHABLE();
		}

		Window* find_window(HWND hwnd) const
		{
			auto it = hwnd_to_vitro_window.find(hwnd);

			if(it == hwnd_to_vitro_window.end())
				return nullptr;

			return it->second;
		}

		void on_window_move(HWND hwnd, LPARAM l_param) const
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

		void on_window_size(HWND hwnd, LPARAM l_param) const
		{
			auto window = find_window(hwnd);
			if(!window)
				return;

			Extent size {
				.width	= LOWORD(l_param),
				.height = HIWORD(l_param),
			};
			EventSystem::notify<WindowSizeEvent>(*window, size);
		}

		void restore_window_cursor_state(HWND hwnd, WPARAM w_param) const
		{
			auto window = find_window(hwnd);
			if(!window || window->cursor_enabled())
				return;

			if(w_param & WA_ACTIVE || w_param & WA_CLICKACTIVE)
				window->disable_cursor();
		}

		template<typename E> void on_window_event(HWND hwnd) const
		{
			auto window = find_window(hwnd);
			if(window)
				EventSystem::notify<E>(*window);
		}

		void on_window_close(HWND hwnd) const
		{
			auto window = find_window(hwnd);
			if(window)
				window->close();
		}

		void on_window_show(HWND hwnd, WPARAM w_param) const
		{
			if(w_param)
				on_window_event<WindowOpenEvent>(hwnd);
			else
				on_window_event<WindowCloseEvent>(hwnd);
		}

		void on_raw_input(HWND hwnd, LPARAM l_param) const
		{
			auto window = find_window(hwnd);
			if(!window)
				return;

			auto input_handle = reinterpret_cast<HRAWINPUT>(l_param);
			UINT size;
			::GetRawInputData(input_handle, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
			Array<BYTE> bytes(size);
			::GetRawInputData(input_handle, RID_INPUT, bytes.data(), &size, sizeof(RAWINPUTHEADER));
			auto input = new(bytes.data()) RAWINPUT;

			Int2 direction {
				.x = input->data.mouse.lLastX,
				.y = input->data.mouse.lLastY,
			};
			EventSystem::notify<MouseMoveEvent>(*window, last_mouse_position, direction);
		}

		void on_key_down(HWND hwnd, WPARAM w_param)
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

		void on_key_up(HWND hwnd, WPARAM w_param)
		{
			key_repeats	  = 0;
			last_key_code = KeyCode::None;

			auto window = find_window(hwnd);
			if(!window)
				return;

			auto key = convert_virtual_key(w_param);
			EventSystem::notify<KeyUpEvent>(*window, key);
		}

		void on_key_text(HWND hwnd, WPARAM w_param) const
		{
			auto window = find_window(hwnd);
			if(!window)
				return;

			wchar_t const chars[] {static_cast<wchar_t>(w_param), L'\0'};
			EventSystem::notify<KeyTextEvent>(*window, last_key_code, narrow_string(chars));
		}

		void store_last_mouse_position(LPARAM l_param)
		{
			last_mouse_position.x = GET_X_LPARAM(l_param);
			last_mouse_position.y = GET_Y_LPARAM(l_param);
		}

		template<typename E> void on_mouse_event(HWND hwnd, MouseCode button) const
		{
			auto window = find_window(hwnd);
			if(window)
				EventSystem::notify<E>(*window, button);
		}

		void on_vertical_scroll(HWND hwnd, WPARAM w_param) const
		{
			auto window = find_window(hwnd);
			if(!window)
				return;

			Float2 offset {
				.y = GET_WHEEL_DELTA_WPARAM(w_param) / float(WHEEL_DELTA),
			};
			EventSystem::notify<MouseScrollEvent>(*window, offset);
		}

		void on_horizontal_scroll(HWND hwnd, WPARAM w_param) const
		{
			auto window = find_window(hwnd);
			if(!window)
				return;

			Float2 offset {
				.x = GET_WHEEL_DELTA_WPARAM(w_param) / float(WHEEL_DELTA),
			};
			EventSystem::notify<MouseScrollEvent>(*window, offset);
		}
	};
}
