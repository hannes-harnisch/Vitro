#include "_pch.h"

#include "ClientBase.h"

#include "Vitro/Client/Input.h"
#include "Vitro/Client/KeyEvent.h"
#include "Vitro/Client/MouseEvent.h"
#include "Vitro/Client/WindowEvent.h"
#include "Vitro/Engine.h"
#include "Vitro/Utility/StackArray.h"
#include <windowsx.h>

// Forward declaration for imgui
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);

namespace Vitro::Windows
{
	std::wstring ClientBase::WidenChars(const std::string& narrow)
	{
		int length = ::MultiByteToWideChar(CP_UTF8, 0, narrow.c_str(), -1, nullptr, 0);
		std::wstring wide(length, '\0');
		::MultiByteToWideChar(CP_UTF8, 0, narrow.c_str(), -1, &wide[0], length);
		return wide;
	}

	std::string ClientBase::NarrowChars(const std::wstring& wide)
	{
		int length = ::WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::string narrow(length, '\0');
		::WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &narrow[0], length, nullptr, nullptr);
		return narrow;
	}

	void ClientBase::SetConsoleColors(uint8_t colorMask)
	{
		static auto outHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		::SetConsoleTextAttribute(outHandle, colorMask);
	}

	ClientBase::ClientBase()
	{
		static bool initialized = false;
		AssertCritical(!initialized, "Windows API already initialized.");

		WNDCLASS windowClass {};
		windowClass.style		  = CS_DBLCLKS;
		windowClass.lpfnWndProc	  = ReceiveMessages;
		windowClass.lpszClassName = WindowClassName;
		windowClass.cbWndExtra	  = sizeof(void*);
		auto wcRes				  = ::RegisterClass(&windowClass);
		AssertCritical(SUCCEEDED(wcRes), "Could not create Windows API window class.");

		RAWINPUTDEVICE rid {};
		rid.usUsagePage = 0x01; // Usage page constant for generic desktop controls
		rid.usUsage		= 0x02; // Usage constant for a generic mouse
		bool ridRes		= ::RegisterRawInputDevices(&rid, 1, sizeof(rid));
		AssertCritical(ridRes, "Could not get raw mouse input device.");

		initialized = true;
	}

	void ClientBase::PollEvents()
	{
		MSG msg;
		while(::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	LRESULT ClientBase::ReceiveMessages(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		return Engine::Get().ForwardMessages(wnd, msg, wp, lp);
	}

	LRESULT ClientBase::ForwardMessages(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		ImGui_ImplWin32_WndProcHandler(wnd, msg, wp, lp);
		auto& w = *reinterpret_cast<Window*>(::GetWindowLongPtr(wnd, 0));
		switch(msg)
		{
			case WM_MOVE: OnWindowMove(w, lp); return 0;
			case WM_SIZE: OnWindowSize(w, lp); return 0;
			case WM_SETFOCUS: OnWindowFocus(w); return 0;
			case WM_KILLFOCUS: OnWindowUnfocus(w); return 0;
			case WM_CLOSE: OnWindowClose(w); return 0;
			case WM_SHOWWINDOW:
				if(wp)
					OnWindowOpen(w);
				return 0;
			case WM_INPUT: OnRawInput(w, lp); return 0;
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN: OnKeyDown(w, wp); return 0;
			case WM_KEYUP:
			case WM_SYSKEYUP: OnKeyUp(w, wp); return 0;
			case WM_CHAR:
			case WM_SYSCHAR: OnKeyText(w, wp); return 0;
			case WM_UNICHAR: OnKeyText(w, wp); return wp == UNICODE_NOCHAR;
			case WM_MOUSEMOVE: OnMouseMove(w, lp); return 0;
			case WM_LBUTTONDOWN: OnMouseDown(w, MouseCode::Mouse1); return 0;
			case WM_RBUTTONDOWN: OnMouseDown(w, MouseCode::Mouse2); return 0;
			case WM_MBUTTONDOWN: OnMouseDown(w, MouseCode::Wheel); return 0;
			case WM_XBUTTONDOWN: OnMouseDown(w, GetMouseExtra(wp)); return true;
			case WM_LBUTTONUP: OnMouseUp(w, MouseCode::Mouse1); return 0;
			case WM_RBUTTONUP: OnMouseUp(w, MouseCode::Mouse2); return 0;
			case WM_MBUTTONUP: OnMouseUp(w, MouseCode::Wheel); return 0;
			case WM_XBUTTONUP: OnMouseUp(w, GetMouseExtra(wp)); return true;
			case WM_LBUTTONDBLCLK: OnDoubleClick(w, MouseCode::Mouse1); return 0;
			case WM_RBUTTONDBLCLK: OnDoubleClick(w, MouseCode::Mouse2); return 0;
			case WM_MBUTTONDBLCLK: OnDoubleClick(w, MouseCode::Wheel); return 0;
			case WM_XBUTTONDBLCLK: OnDoubleClick(w, GetMouseExtra(wp)); return true;
			case WM_MOUSEWHEEL: OnMouseScrollVertical(w, wp); return 0;
			case WM_MOUSEHWHEEL: OnMouseScrollHorizontal(w, wp); return 0;
			default: return ::DefWindowProc(wnd, msg, wp, lp);
		}
	}

	void ClientBase::OnWindowMove(Window& window, LPARAM lp)
	{
		WindowMoveEvent e(window, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
		window.OnEvent(e);
	}

	void ClientBase::OnWindowSize(Window& window, LPARAM lp)
	{
		WindowSizeEvent e(window, LOWORD(lp), HIWORD(lp));
		window.OnEvent(e);
	}

	void ClientBase::OnWindowFocus(Window& window)
	{
		WindowFocusEvent e(window);
		window.OnEvent(e);
	}

	void ClientBase::OnWindowUnfocus(Window& window)
	{
		WindowUnfocusEvent e(window);
		window.OnEvent(e);
	}

	void ClientBase::OnWindowClose(Window& window)
	{
		WindowCloseEvent e(window);
		window.OnEvent(e);
		EraseOpenWindow(window);
		window.Close();
	}

	void ClientBase::OnWindowOpen(Window& window)
	{
		WindowOpenEvent e(window);
		window.OnEvent(e);
		EmplaceOpenWindow(window);
	}

	void ClientBase::OnRawInput(Window& window, LPARAM lp)
	{
		UINT size = 0;
		::GetRawInputData(reinterpret_cast<HRAWINPUT>(lp), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
		StackArray<BYTE> bytes(size);
		::GetRawInputData(reinterpret_cast<HRAWINPUT>(lp), RID_INPUT, bytes.Raw(), &size, sizeof(RAWINPUTHEADER));
		auto input = new(bytes.Raw()) RAWINPUT;

		MouseMoveEvent e(input->data.mouse.lLastX, -input->data.mouse.lLastY);
		window.OnEvent(e);
	}

	void ClientBase::OnKeyDown(Window& window, WPARAM wp)
	{
		auto key = static_cast<KeyCode>(wp);
		if(LastKeyCode == key)
			KeyRepeats++;
		else
			KeyRepeats = 0;
		LastKeyCode			  = key;
		Input::KeyStates[key] = true;
		KeyDownEvent e(key, KeyRepeats);
		window.OnEvent(e);
	}

	void ClientBase::OnKeyUp(Window& window, WPARAM wp)
	{
		LastKeyCode			  = KeyCode::None;
		KeyRepeats			  = 0;
		KeyCode key			  = static_cast<KeyCode>(wp);
		Input::KeyStates[key] = false;
		KeyUpEvent e(key);
		window.OnEvent(e);
	}

	void ClientBase::OnKeyText(Window& window, WPARAM wp)
	{
		auto character = NarrowChars({static_cast<wchar_t>(wp)});
		character.resize(character.find('\0'));
		KeyTextEvent e(LastKeyCode, character);
		window.OnEvent(e);
	}

	void ClientBase::OnMouseMove(Window& window, LPARAM lp)
	{
		int x = GET_X_LPARAM(lp);
		int y = GET_Y_LPARAM(lp);

		Input::MousePosition = Int2(x, y);
	}

	void ClientBase::OnMouseDown(Window& window, MouseCode button)
	{
		Input::MouseStates[button] = true;
		MouseDownEvent e(button);
		window.OnEvent(e);
	}

	void ClientBase::OnMouseUp(Window& window, MouseCode button)
	{
		Input::MouseStates[button] = false;
		MouseUpEvent e(button);
		window.OnEvent(e);
	}

	void ClientBase::OnDoubleClick(Window& window, MouseCode button)
	{
		DoubleClickEvent e(button);
		window.OnEvent(e);
	}

	void ClientBase::OnMouseScrollVertical(Window& window, WPARAM wp)
	{
		float yOffset = short(HIWORD(wp)) / float(WHEEL_DELTA);
		MouseScrollEvent e(0, yOffset);
		window.OnEvent(e);
	}

	void ClientBase::OnMouseScrollHorizontal(Window& window, WPARAM wp)
	{
		// X axis is inverted for consistency with Linux and macOS.
		float xOffset = short(HIWORD(wp)) / -float(WHEEL_DELTA);
		MouseScrollEvent e(xOffset, 0);
		window.OnEvent(e);
	}

	MouseCode ClientBase::GetMouseExtra(WPARAM wp)
	{
		return static_cast<MouseCode>(HIWORD(wp) + 3);
	}
}
