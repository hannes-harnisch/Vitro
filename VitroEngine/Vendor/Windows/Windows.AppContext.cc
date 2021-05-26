module;
#include "Trace/Assert.hh"
#include "Windows.API.hh"
export module Vitro.Windows.AppContext;

import Vitro.App.IAppContext;
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
	export class AppContext final : public IAppContext
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
		const HINSTANCE instanceHandle;
		KeyCode lastKeyCode {};
		unsigned keyRepeats {};
		Int2 lastMousePosition;

		static LRESULT CALLBACK forwardMessages(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch(message)
			{
				case WM_MOVE: notifyWindowMoveEvent(hwnd, lParam); return 0;
				case WM_SIZE: notifyWindowSizeEvent(hwnd, lParam); return 0;
				case WM_SETFOCUS: notifyWindowEvent<WindowFocusEvent>(hwnd); return 0;
				case WM_KILLFOCUS: notifyWindowEvent<WindowUnfocusEvent>(hwnd); return 0;
				case WM_CLOSE: notifyWindowEvent<WindowCloseEvent>(hwnd); return 0;
				case WM_SHOWWINDOW:
					if(wParam)
						notifyWindowEvent<WindowOpenEvent>(hwnd);
					return 0;
				case WM_INPUT: notifyRawInput(hwnd, lParam); return 0;
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN: notifyKeyDownEvent(hwnd, wParam); return 0;
				case WM_KEYUP:
				case WM_SYSKEYUP: notifyKeyUpEvent(hwnd, wParam); return 0;
				case WM_CHAR:
				case WM_SYSCHAR: notifyKeyTextEvent(hwnd, wParam); return 0;
				case WM_UNICHAR: notifyKeyTextEvent(hwnd, wParam); return wParam == UNICODE_NOCHAR;
				case WM_MOUSEMOVE: storeLastMousePosition(lParam); return 0;
				case WM_LBUTTONDOWN: notifyMouseEvent<MouseDownEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONDOWN: notifyMouseEvent<MouseDownEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONDOWN: notifyMouseEvent<MouseDownEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONDOWN: notifyMouseEvent<MouseDownEvent>(hwnd, getExtraMouseButton(wParam)); return true;
				case WM_LBUTTONUP: notifyMouseEvent<MouseUpEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONUP: notifyMouseEvent<MouseUpEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONUP: notifyMouseEvent<MouseUpEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONUP: notifyMouseEvent<MouseUpEvent>(hwnd, getExtraMouseButton(wParam)); return true;
				case WM_LBUTTONDBLCLK: notifyMouseEvent<DoubleClickEvent>(hwnd, MouseCode::Mouse1); return 0;
				case WM_RBUTTONDBLCLK: notifyMouseEvent<DoubleClickEvent>(hwnd, MouseCode::Mouse2); return 0;
				case WM_MBUTTONDBLCLK: notifyMouseEvent<DoubleClickEvent>(hwnd, MouseCode::Wheel); return 0;
				case WM_XBUTTONDBLCLK: notifyMouseEvent<DoubleClickEvent>(hwnd, getExtraMouseButton(wParam)); return true;
				case WM_MOUSEWHEEL: notifyVerticalScrollEvent(hwnd, wParam); return 0;
				case WM_MOUSEHWHEEL: notifyHorizontalScrollEvent(hwnd, wParam); return 0;
			}
			return ::DefWindowProc(hwnd, message, wParam, lParam);
		}

		static void notifyWindowMoveEvent(HWND hwnd, LPARAM lp)
		{
			auto& window = *get().findWindow(hwnd);
			Int2 position {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
			EventSystem::get().notify<WindowMoveEvent>(window, position);
		}

		static void notifyWindowSizeEvent(HWND hwnd, LPARAM lp)
		{
			auto& window = *get().findWindow(hwnd);
			Rectangle size(LOWORD(lp), HIWORD(lp));
			EventSystem::get().notify<WindowSizeEvent>(window, size);
		}

		template<typename E> static void notifyWindowEvent(HWND hwnd)
		{
			auto window = get().findWindow(hwnd);
			if(window)
				EventSystem::get().notify<E>(*window);
		}

		static void notifyRawInput(HWND hwnd, LPARAM lp)
		{
			UINT size {};
			auto inputHandle = reinterpret_cast<HRAWINPUT>(lp);

			::GetRawInputData(inputHandle, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
			Array<BYTE> bytes(size);
			::GetRawInputData(inputHandle, RID_INPUT, bytes.data(), &size, sizeof(RAWINPUTHEADER));
			auto input = new(bytes.data()) RAWINPUT;

			auto& self	 = static_cast<AppContext&>(get());
			auto& window = *self.findWindow(hwnd);
			Int2 direction {input->data.mouse.lLastX, -input->data.mouse.lLastY};
			EventSystem::get().notify<MouseMoveEvent>(window, self.lastMousePosition, direction);
		}

		static void notifyKeyDownEvent(HWND hwnd, WPARAM wp)
		{
			auto& self	 = static_cast<AppContext&>(get());
			auto& window = *self.findWindow(hwnd);
			auto key	 = static_cast<KeyCode>(wp);

			if(self.lastKeyCode == key)
				self.keyRepeats++;
			else
				self.keyRepeats = 0;
			self.lastKeyCode = key;

			EventSystem::get().notify<KeyDownEvent>(window, key, self.keyRepeats);
		}

		static void notifyKeyUpEvent(HWND hwnd, WPARAM wp)
		{
			auto& self		 = static_cast<AppContext&>(get());
			auto& window	 = *self.findWindow(hwnd);
			self.lastKeyCode = KeyCode::None;
			self.keyRepeats	 = 0;

			auto key = static_cast<KeyCode>(wp);
			EventSystem::get().notify<KeyUpEvent>(window, key);
		}

		static void notifyKeyTextEvent(HWND hwnd, WPARAM wp)
		{
			auto& self	 = static_cast<AppContext&>(get());
			auto& window = *self.findWindow(hwnd);

			wchar_t chars[] {static_cast<wchar_t>(wp), L'\0'};
			EventSystem::get().notify<KeyTextEvent>(window, self.lastKeyCode, narrowString(chars));
		}

		static void storeLastMousePosition(LPARAM lp)
		{
			auto& self = static_cast<AppContext&>(get());

			self.lastMousePosition.x = GET_X_LPARAM(lp);
			self.lastMousePosition.y = GET_Y_LPARAM(lp);
		}

		template<typename E> static void notifyMouseEvent(HWND hwnd, MouseCode button)
		{
			auto& window = *get().findWindow(hwnd);
			EventSystem::get().notify<E>(window, button);
		}

		static MouseCode getExtraMouseButton(WPARAM wp)
		{
			return static_cast<MouseCode>(HIWORD(wp) + 3);
		}

		static void notifyVerticalScrollEvent(HWND hwnd, WPARAM wp)
		{
			auto& window = *get().findWindow(hwnd);
			Float2 offset {0.0f, short(HIWORD(wp)) / float(WHEEL_DELTA)};
			EventSystem::get().notify<MouseScrollEvent>(window, offset);
		}

		static void notifyHorizontalScrollEvent(HWND hwnd, WPARAM wp)
		{
			auto& window = *get().findWindow(hwnd);
			Float2 offset {short(HIWORD(wp)) / -float(WHEEL_DELTA), 0.0f};
			EventSystem::get().notify<MouseScrollEvent>(window, offset);
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
	};
}
