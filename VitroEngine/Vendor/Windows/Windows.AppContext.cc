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

class AppSystem;
namespace Windows
{
	export class AppContext final : public AppContextBase
	{
		friend ::AppSystem;

	public:
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
		KeyCode lastKeyCode {};
		unsigned keyRepeats {};
		Int2 lastMousePosition;

		static AppContext& get()
		{
			return static_cast<AppContext&>(AppContextBase::get());
		}

		static LRESULT CALLBACK forwardMessages(HWND const hwnd, UINT const message, WPARAM const wParam, LPARAM const lParam)
		{
			switch(message)
			{
				case WM_MOVE: get().notifyWindowMoveEvent(hwnd, lParam); return 0;
				case WM_SIZE: get().notifyWindowSizeEvent(hwnd, lParam); return 0;
				case WM_ACTIVATE: get().restoreWindowCursorState(hwnd, wParam); return 0;
				case WM_SETFOCUS: get().notifyWindowEvent<WindowFocusEvent>(hwnd); return 0;
				case WM_KILLFOCUS: get().notifyWindowEvent<WindowUnfocusEvent>(hwnd); return 0;
				case WM_CLOSE: get().notifyWindowEvent<WindowCloseEvent>(hwnd); return 0;
				case WM_SHOWWINDOW:
					if(wParam)
						get().notifyWindowEvent<WindowOpenEvent>(hwnd);
					return 0;
				case WM_INPUT: get().notifyRawInput(hwnd, lParam); return 0;
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN: get().notifyKeyDownEvent(hwnd, wParam); return 0;
				case WM_KEYUP:
				case WM_SYSKEYUP: get().notifyKeyUpEvent(hwnd, wParam); return 0;
				case WM_CHAR:
				case WM_SYSCHAR: get().notifyKeyTextEvent(hwnd, wParam); return 0;
				case WM_UNICHAR: get().notifyKeyTextEvent(hwnd, wParam); return wParam == UNICODE_NOCHAR;
				case WM_MOUSEMOVE: get().storeLastMousePosition(lParam); return 0;
				case WM_LBUTTONDOWN: get().notifyMouseEvent<MouseDownEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONDOWN: get().notifyMouseEvent<MouseDownEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONDOWN: get().notifyMouseEvent<MouseDownEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONDOWN: get().notifyMouseEvent<MouseDownEvent>(hwnd, getExtraMouseButton(wParam)); return true;
				case WM_LBUTTONUP: get().notifyMouseEvent<MouseUpEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONUP: get().notifyMouseEvent<MouseUpEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONUP: get().notifyMouseEvent<MouseUpEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONUP: get().notifyMouseEvent<MouseUpEvent>(hwnd, getExtraMouseButton(wParam)); return true;
				case WM_LBUTTONDBLCLK: get().notifyMouseEvent<DoubleClickEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONDBLCLK: get().notifyMouseEvent<DoubleClickEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONDBLCLK: get().notifyMouseEvent<DoubleClickEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONDBLCLK: get().notifyMouseEvent<DoubleClickEvent>(hwnd, getExtraMouseButton(wParam)); return true;
				case WM_MOUSEWHEEL: get().notifyVerticalScrollEvent(hwnd, wParam); return 0;
				case WM_MOUSEHWHEEL: get().notifyHorizontalScrollEvent(hwnd, wParam); return 0;
			}
			return ::DefWindowProc(hwnd, message, wParam, lParam);
		}

		static MouseCode getExtraMouseButton(WPARAM const wp)
		{
			return static_cast<MouseCode>(HIWORD(wp) + 3);
		}

		AppContext() : instanceHandle(::GetModuleHandle(nullptr))
		{
			WNDCLASS windowClass {};
			windowClass.style		  = CS_DBLCLKS;
			windowClass.lpfnWndProc	  = forwardMessages;
			windowClass.hInstance	  = instanceHandle;
			windowClass.lpszClassName = Window::WindowClassName;
			veEnsure(::RegisterClass(&windowClass), "Failed to register window class.");

			RAWINPUTDEVICE rawInputDevice {};
			rawInputDevice.usUsagePage = 0x01; // Usage page constant for generic desktop controls
			rawInputDevice.usUsage	   = 0x02; // Usage constant for a generic mouse
			veEnsure(::RegisterRawInputDevices(&rawInputDevice, 1, sizeof(RAWINPUTDEVICE)),
					 "Failed to register raw input device.");
		}

		void notifyWindowMoveEvent(HWND const hwnd, LPARAM const lp)
		{
			auto& window = *findWindow(hwnd);
			Int2 position {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};

			EventSystem::get().notify<WindowMoveEvent>(window, position);
		}

		void notifyWindowSizeEvent(HWND const hwnd, LPARAM const lp)
		{
			auto& window = *findWindow(hwnd);
			Rectangle const size(LOWORD(lp), HIWORD(lp));

			EventSystem::get().notify<WindowSizeEvent>(window, size);
		}

		void restoreWindowCursorState(HWND const hwnd, WPARAM const wp)
		{
			auto window = findWindow(hwnd);
			if(!window || window->cursorEnabled())
				return;

			if(wp & WA_ACTIVE || wp & WA_CLICKACTIVE)
				window->disableCursor();
			else
				window->enableCursor();
		}

		template<typename E> void notifyWindowEvent(HWND const hwnd)
		{
			auto window = findWindow(hwnd);
			if(window)
				EventSystem::get().notify<E>(*window);
		}

		void notifyRawInput(HWND const hwnd, LPARAM const lp)
		{
			UINT size {};
			auto const inputHandle = reinterpret_cast<HRAWINPUT>(lp);

			::GetRawInputData(inputHandle, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
			Array<BYTE> bytes(size);
			::GetRawInputData(inputHandle, RID_INPUT, bytes.data(), &size, sizeof(RAWINPUTHEADER));
			auto input = new(bytes.data()) RAWINPUT;

			auto& window = *findWindow(hwnd);
			Int2 const direction {input->data.mouse.lLastX, input->data.mouse.lLastY};
			EventSystem::get().notify<MouseMoveEvent>(window, lastMousePosition, direction);
		}

		void notifyKeyDownEvent(HWND const hwnd, WPARAM const wp)
		{
			auto const key = static_cast<KeyCode>(wp);

			if(lastKeyCode == key)
				keyRepeats++;
			else
				keyRepeats = 0;
			lastKeyCode = key;

			auto& window = *findWindow(hwnd);
			EventSystem::get().notify<KeyDownEvent>(window, key, keyRepeats);
		}

		void notifyKeyUpEvent(HWND const hwnd, WPARAM const wp)
		{
			lastKeyCode = KeyCode::None;
			keyRepeats	= 0;

			auto& window   = *findWindow(hwnd);
			auto const key = static_cast<KeyCode>(wp);
			EventSystem::get().notify<KeyUpEvent>(window, key);
		}

		void notifyKeyTextEvent(HWND const hwnd, WPARAM const wp)
		{
			auto& window = *findWindow(hwnd);
			wchar_t const chars[] {static_cast<wchar_t>(wp), L'\0'};

			EventSystem::get().notify<KeyTextEvent>(window, lastKeyCode, narrowString(chars));
		}

		void storeLastMousePosition(LPARAM const lp)
		{
			lastMousePosition.x = GET_X_LPARAM(lp);
			lastMousePosition.y = GET_Y_LPARAM(lp);
		}

		template<typename E> void notifyMouseEvent(HWND const hwnd, MouseCode const button)
		{
			auto& window = *findWindow(hwnd);
			EventSystem::get().notify<E>(window, button);
		}

		void notifyVerticalScrollEvent(HWND const hwnd, WPARAM const wp)
		{
			auto& window = *findWindow(hwnd);
			Float2 const offset {0.0f, short(HIWORD(wp)) / float(WHEEL_DELTA)};

			EventSystem::get().notify<MouseScrollEvent>(window, offset);
		}

		void notifyHorizontalScrollEvent(HWND const hwnd, WPARAM const wp)
		{
			auto& window = *findWindow(hwnd);
			Float2 const offset {short(HIWORD(wp)) / -float(WHEEL_DELTA), 0.0f};

			EventSystem::get().notify<MouseScrollEvent>(window, offset);
		}
	};
}
