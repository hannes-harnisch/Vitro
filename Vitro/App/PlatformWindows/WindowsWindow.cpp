module;
#include "VitroCore/Macros.hpp"
#include "VitroCore/PlatformWindows/WindowsAPI.hpp"

#include <memory>
#include <string_view>
module vt.App.Windows.Window;

import vt.App.AppContextBase;
import vt.Core.Rect;
import vt.Core.Vector;
import vt.Core.Windows.Utils;

namespace vt::windows
{
	HWND make_window(std::string_view title, Rectangle rect)
	{
		auto widened_title = widen_string(title);
		auto instance	   = AppContextBase::get_system_window_owner();

		auto hwnd = ::CreateWindowEx(WS_EX_APPWINDOW, WindowsWindow::WINDOW_CLASS_NAME, widened_title.data(),
									 WS_OVERLAPPEDWINDOW, rect.x, rect.y, rect.width, rect.height, nullptr, nullptr, instance,
									 nullptr);
		check_winapi_error(hwnd, "Failed to create window.");
		return hwnd;
	}

	WindowsWindow::WindowsWindow(std::string_view title, Rectangle rect) : window(make_window(title, rect))
	{}

	void WindowsWindow::open()
	{
		::ShowWindow(window.get(), SW_RESTORE);
	}

	void WindowsWindow::close()
	{
		::ShowWindow(window.get(), SW_HIDE);
	}

	void WindowsWindow::maximize()
	{
		::ShowWindow(window.get(), SW_MAXIMIZE);
	}

	void WindowsWindow::minimize()
	{
		::ShowWindow(window.get(), SW_MINIMIZE);
	}

	void WindowsWindow::enable_cursor()
	{
		while(::ShowCursor(true) < 0)
		{}

		auto succeeded = ::ClipCursor(nullptr);
		check_winapi_error(succeeded, "Failed to clip cursor.");
	}

	void WindowsWindow::disable_cursor()
	{
		while(::ShowCursor(false) >= 0)
		{}

		RECT rect;
		auto succeeded = ::GetClientRect(window.get(), &rect);
		check_winapi_error(succeeded, "Failed to get window client rect.");

		::SetLastError(0);
		succeeded = ::MapWindowPoints(window.get(), nullptr, reinterpret_cast<POINT*>(&rect), sizeof(RECT) / sizeof(POINT));
		check_winapi_error(succeeded, "Failed to map window points.");

		succeeded = ::ClipCursor(&rect);
		check_winapi_error(succeeded, "Failed to clip cursor.");
	}

	void WindowsWindow::enable_resize()
	{
		make_resizable(true);
	}

	void WindowsWindow::disable_resize()
	{
		make_resizable(false);
	}

	Extent WindowsWindow::get_size() const
	{
		RECT rect;

		auto succeeded = ::GetWindowRect(window.get(), &rect);
		check_winapi_error(succeeded, "Failed to get window rect.");

		return {
			.width	= static_cast<unsigned>(rect.right - rect.left),
			.height = static_cast<unsigned>(rect.bottom - rect.top),
		};
	}

	void WindowsWindow::set_size(Extent size)
	{
		auto succeeded = ::SetWindowPos(window.get(), nullptr, 0, 0, size.width, size.height, SWP_NOMOVE | SWP_NOZORDER);
		check_winapi_error(succeeded, "Failed to set window size.");
	}

	Int2 WindowsWindow::get_position() const
	{
		RECT rect;

		auto succeeded = ::GetWindowRect(window.get(), &rect);
		check_winapi_error(succeeded, "Failed to get window rect.");

		return {
			.x = rect.left,
			.y = rect.top,
		};
	}

	void WindowsWindow::set_position(Int2 position)
	{
		auto succeeded = ::SetWindowPos(window.get(), nullptr, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		check_winapi_error(succeeded, "Failed to set window position.");
	}

	std::string WindowsWindow::get_title() const
	{
		::SetLastError(0);
		int length = ::GetWindowTextLength(window.get());
		check_winapi_error(length, "Failed to get window title length.");

		std::wstring title(length, L'\0');
		length = ::GetWindowText(window.get(), title.data(), length + 1);
		check_winapi_error(length, "Failed to get window title.");

		return narrow_string(title);
	}

	void WindowsWindow::set_title(std::string_view title)
	{
		auto widened_title = widen_string(title);

		auto succeeded = ::SetWindowText(window.get(), widened_title.data());
		check_winapi_error(succeeded, "Failed to set window title.");
	}

	Rectangle WindowsWindow::client_area() const
	{
		RECT rect;

		auto succeeded = ::GetClientRect(window.get(), &rect);
		check_winapi_error(succeeded, "Failed to get window client rect.");

		return {
			.x		= rect.left,
			.y		= rect.top,
			.width	= static_cast<unsigned>(rect.right - rect.left),
			.height = static_cast<unsigned>(rect.bottom - rect.top),
		};
	}

	HWND WindowsWindow::native_handle()
	{
		return window.get();
	}

	void WindowsWindow::make_resizable(bool enable) const
	{
		auto style = ::GetWindowLongPtr(window.get(), GWL_STYLE);
		check_winapi_error(style, "Failed to get Windows window attributes.");

		if(enable)
			style |= WS_THICKFRAME;
		else
			style ^= WS_THICKFRAME;

		::SetLastError(0);
		auto result = ::SetWindowLongPtr(window.get(), GWL_STYLE, style);
		check_winapi_error(result, "Failed to set Windows window attributes.");
	}
}
