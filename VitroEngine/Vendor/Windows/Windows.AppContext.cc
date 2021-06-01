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

namespace Windows
{
	export class AppContext final : public AppContextBase
	{
	public:
		static AppContext& get()
		{
			return static_cast<AppContext&>(AppContextBase::get());
		}

		void pollEvents() const final override
		{
			MSG message;
			while(::PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&message);
				::DispatchMessage(&message);
			}
		}

		void* handle() final override
		{
			return instanceHandle;
		}

	protected:
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

	private:
		HINSTANCE const instanceHandle;
		KeyCode lastKeyCode {};
		uint32_t keyRepeats {};
		Int2 lastMousePosition;

		static LRESULT CALLBACK forwardMessages(HWND const hwnd, UINT const message, WPARAM const wParam, LPARAM const lParam)
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
				case WM_INPUT: get().onRawInput(hwnd, lParam); return 0;
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

		static MouseCode mapExtraMouseButton(WPARAM const wp)
		{
			return static_cast<MouseCode>(HIWORD(wp) + 3);
		}

		void onWindowMove(HWND const hwnd, LPARAM const lp)
		{
			auto& window = *findWindow(hwnd);
			Int2 position {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};

			EventSystem::get().notify<WindowMoveEvent>(window, position);
		}

		void onWindowSize(HWND const hwnd, LPARAM const lp)
		{
			auto& window = *findWindow(hwnd);
			Rectangle const size(LOWORD(lp), HIWORD(lp));

			EventSystem::get().notify<WindowSizeEvent>(window, size);
		}

		void restoreWindowCursorState(HWND const hwnd, WPARAM const wp)
		{
			auto& window = *findWindow(hwnd);
			if(window.cursorEnabled())
				return;

			if(wp & WA_ACTIVE || wp & WA_CLICKACTIVE)
				window.disableCursor();
		}

		template<typename E> void onWindowEvent(HWND const hwnd)
		{
			auto const window = findWindow(hwnd);
			if(window)
				EventSystem::get().notify<E>(*window);
		}

		void onWindowClose(HWND const hwnd)
		{
			findWindow(hwnd)->close();
		}

		void onWindowShow(HWND const hwnd, WPARAM const wp)
		{
			if(wp)
				onWindowEvent<WindowOpenEvent>(hwnd);
			else
				onWindowEvent<WindowCloseEvent>(hwnd);
		}

		void onRawInput(HWND const hwnd, LPARAM const lp)
		{
			UINT size {};
			auto const inputHandle = reinterpret_cast<HRAWINPUT>(lp);

			::GetRawInputData(inputHandle, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
			Array<BYTE> bytes(size);
			::GetRawInputData(inputHandle, RID_INPUT, bytes.data(), &size, sizeof(RAWINPUTHEADER));
			auto const input = new(bytes.data()) RAWINPUT;

			auto& window = *findWindow(hwnd);
			Int2 const direction {input->data.mouse.lLastX, input->data.mouse.lLastY};
			EventSystem::get().notify<MouseMoveEvent>(window, lastMousePosition, direction);
		}

		void onKeyDown(HWND const hwnd, WPARAM const wp)
		{
			auto const key = static_cast<KeyCode>(wp);

			if(lastKeyCode == key)
				++keyRepeats;
			else
				keyRepeats = 0;
			lastKeyCode = key;

			auto& window = *findWindow(hwnd);
			EventSystem::get().notify<KeyDownEvent>(window, key, keyRepeats);
		}

		void onKeyUp(HWND const hwnd, WPARAM const wp)
		{
			lastKeyCode = KeyCode::None;
			keyRepeats	= 0;

			auto& window   = *findWindow(hwnd);
			auto const key = static_cast<KeyCode>(wp);
			EventSystem::get().notify<KeyUpEvent>(window, key);
		}

		void onKeyText(HWND const hwnd, WPARAM const wp)
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

		template<typename E> void onMouseEvent(HWND const hwnd, MouseCode const button)
		{
			auto& window = *findWindow(hwnd);
			EventSystem::get().notify<E>(window, button);
		}

		void onVerticalScroll(HWND const hwnd, WPARAM const wp)
		{
			auto& window = *findWindow(hwnd);
			Float2 const offset {0.0f, short(HIWORD(wp)) / float(WHEEL_DELTA)};

			EventSystem::get().notify<MouseScrollEvent>(window, offset);
		}

		void onHorizontalScroll(HWND const hwnd, WPARAM const wp)
		{
			auto& window = *findWindow(hwnd);
			Float2 const offset {short(HIWORD(wp)) / -float(WHEEL_DELTA), 0.0f};

			EventSystem::get().notify<MouseScrollEvent>(window, offset);
		}
	};
}
