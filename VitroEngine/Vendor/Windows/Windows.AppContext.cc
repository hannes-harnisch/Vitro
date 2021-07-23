module;
#include "Trace/Assert.hh"
#include "Windows.API.hh"
export module Vitro.Windows.AppContext;

import Vitro.App.AppContextBase;
import Vitro.App.EventSystem;
import Vitro.App.KeyCode;
import Vitro.App.MouseCode;
import Vitro.App.Window;
import Vitro.App.WindowEvent;
import Vitro.Core.Array;
import Vitro.Math.Rectangle;
import Vitro.Math.Vector;
import Vitro.Windows.StringUtils;

namespace vt::windows
{
	export class AppContext final : public AppContextBase
	{
	public:
		static AppContext& get()
		{
			return static_cast<AppContext&>(AppContextBase::get());
		}

		AppContext() : instanceHandle(::GetModuleHandle(nullptr))
		{
			WNDCLASS const windowClass {
				.style		   = CS_DBLCLKS,
				.lpfnWndProc   = forwardMessages,
				.hInstance	   = instanceHandle,
				.lpszClassName = Window::WindowClassName,
			};
			ATOM registered = ::RegisterClass(&windowClass);
			vtEnsure(registered, "Failed to register window class.");

			RAWINPUTDEVICE const rawInputDevice {
				.usUsagePage = 0x01, // Usage page constant for generic desktop controls
				.usUsage	 = 0x02, // Usage constant for a generic mouse
			};
			BOOL succeeded = ::RegisterRawInputDevices(&rawInputDevice, 1, sizeof(RAWINPUTDEVICE));
			vtEnsure(succeeded, "Failed to register raw input device.");
		}

		void pollEvents() const override
		{
			MSG message;
			while(::PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&message);
				::DispatchMessage(&message);
			}
		}

		void* handle() override
		{
			return instanceHandle;
		}

	private:
		HINSTANCE const instanceHandle;
		KeyCode lastKeyCode = KeyCode::None;
		unsigned keyRepeats = 0;
		Int2 lastMousePosition {};

		static LRESULT CALLBACK forwardMessages(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch(message)
			{
				case WM_MOVE: get().onWindowMove(hwnd, lParam); return 0;
				case WM_SIZE: get().onWindowSize(hwnd, lParam); return 0;
				case WM_ACTIVATE: get().restoreWindowCursorState(hwnd, wParam); return 0;
				case WM_SETFOCUS: get().onWindowEvent<WindowFocusEvent>(hwnd); return 0;
				case WM_KILLFOCUS: get().onWindowEvent<WindowUnfocusEvent>(hwnd); return 0;
				case WM_CLOSE: get().onWindowClose(hwnd); return 0;
				case WM_SHOWWINDOW: get().onWindowShow(hwnd, wParam); return 0;
				case WM_INPUT: get().onRawInput(hwnd, lParam); break;
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN: get().onKeyDown(hwnd, wParam); return 0;
				case WM_KEYUP:
				case WM_SYSKEYUP: get().onKeyUp(hwnd, wParam); return 0;
				case WM_CHAR:
				case WM_SYSCHAR: get().onKeyText(hwnd, wParam); return 0;
				case WM_UNICHAR: get().onKeyText(hwnd, wParam); return wParam == UNICODE_NOCHAR;
				case WM_MOUSEMOVE: get().storeLastMousePosition(lParam); return 0;
				case WM_LBUTTONDOWN: get().onMouseEvent<MouseDownEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONDOWN: get().onMouseEvent<MouseDownEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONDOWN: get().onMouseEvent<MouseDownEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONDOWN: get().onMouseEvent<MouseDownEvent>(hwnd, mapExtraMouseButton(wParam)); return true;
				case WM_LBUTTONUP: get().onMouseEvent<MouseUpEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONUP: get().onMouseEvent<MouseUpEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONUP: get().onMouseEvent<MouseUpEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONUP: get().onMouseEvent<MouseUpEvent>(hwnd, mapExtraMouseButton(wParam)); return true;
				case WM_LBUTTONDBLCLK: get().onMouseEvent<DoubleClickEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONDBLCLK: get().onMouseEvent<DoubleClickEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONDBLCLK: get().onMouseEvent<DoubleClickEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONDBLCLK: get().onMouseEvent<DoubleClickEvent>(hwnd, mapExtraMouseButton(wParam)); return true;
				case WM_MOUSEWHEEL: get().onVerticalScroll(hwnd, wParam); return 0;
				case WM_MOUSEHWHEEL: get().onHorizontalScroll(hwnd, wParam); return 0;
			}
			return ::DefWindowProc(hwnd, message, wParam, lParam);
		}

		static MouseCode mapExtraMouseButton(WPARAM wp)
		{
			return static_cast<MouseCode>(HIWORD(wp) + 3);
		}

		void onWindowMove(HWND hwnd, LPARAM lp)
		{
			auto window = findWindow(hwnd);
			if(!window)
				return;

			Int2 position {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
			EventSystem::notify<WindowMoveEvent>(*window, position);
		}

		void onWindowSize(HWND hwnd, LPARAM lp)
		{
			auto window = findWindow(hwnd);
			if(!window)
				return;

			Extent size(LOWORD(lp), HIWORD(lp));
			EventSystem::notify<WindowSizeEvent>(*window, size);
		}

		void restoreWindowCursorState(HWND hwnd, WPARAM wp)
		{
			auto window = findWindow(hwnd);
			if(!window || window->cursorEnabled())
				return;

			if(wp & WA_ACTIVE || wp & WA_CLICKACTIVE)
				window->disableCursor();
		}

		template<typename E> void onWindowEvent(HWND hwnd)
		{
			auto window = findWindow(hwnd);
			if(window)
				EventSystem::notify<E>(*window);
		}

		void onWindowClose(HWND hwnd)
		{
			auto window = findWindow(hwnd);
			if(window)
				window->close();
		}

		void onWindowShow(HWND hwnd, WPARAM wp)
		{
			if(wp)
				onWindowEvent<WindowOpenEvent>(hwnd);
			else
				onWindowEvent<WindowCloseEvent>(hwnd);
		}

		void onRawInput(HWND hwnd, LPARAM lp)
		{
			auto window = findWindow(hwnd);
			if(!window)
				return;

			UINT size;
			auto inputHandle = reinterpret_cast<HRAWINPUT>(lp);

			::GetRawInputData(inputHandle, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
			Array<BYTE> bytes(size);
			::GetRawInputData(inputHandle, RID_INPUT, bytes.data(), &size, sizeof(RAWINPUTHEADER));
			auto input = new(bytes.data()) RAWINPUT;

			Int2 direction {input->data.mouse.lLastX, input->data.mouse.lLastY};
			EventSystem::notify<MouseMoveEvent>(*window, lastMousePosition, direction);
		}

		void onKeyDown(HWND hwnd, WPARAM wp)
		{
			auto key = static_cast<KeyCode>(wp);

			if(key == lastKeyCode)
				++keyRepeats;
			else
				keyRepeats = 0;
			lastKeyCode = key;

			auto window = findWindow(hwnd);
			if(window)
				EventSystem::notify<KeyDownEvent>(*window, key, keyRepeats);
		}

		void onKeyUp(HWND hwnd, WPARAM wp)
		{
			lastKeyCode = KeyCode::None;
			keyRepeats	= 0;

			auto window = findWindow(hwnd);
			if(!window)
				return;

			auto key = static_cast<KeyCode>(wp);
			EventSystem::notify<KeyUpEvent>(*window, key);
		}

		void onKeyText(HWND hwnd, WPARAM wp)
		{
			auto window = findWindow(hwnd);
			if(!window)
				return;

			wchar_t const chars[] {static_cast<wchar_t>(wp), L'\0'};
			EventSystem::notify<KeyTextEvent>(*window, lastKeyCode, narrowString(chars));
		}

		void storeLastMousePosition(LPARAM lp)
		{
			lastMousePosition.x = GET_X_LPARAM(lp);
			lastMousePosition.y = GET_Y_LPARAM(lp);
		}

		template<typename E> void onMouseEvent(HWND hwnd, MouseCode button)
		{
			auto window = findWindow(hwnd);
			if(window)
				EventSystem::notify<E>(*window, button);
		}

		void onVerticalScroll(HWND hwnd, WPARAM wp)
		{
			auto window = findWindow(hwnd);
			if(!window)
				return;

			Float2 offset {0.0f, short(HIWORD(wp)) / float(WHEEL_DELTA)};
			EventSystem::notify<MouseScrollEvent>(*window, offset);
		}

		void onHorizontalScroll(HWND hwnd, WPARAM wp)
		{
			auto window = findWindow(hwnd);
			if(!window)
				return;

			Float2 offset {short(HIWORD(wp)) / -float(WHEEL_DELTA), 0.0f};
			EventSystem::notify<MouseScrollEvent>(*window, offset);
		}
	};
}
