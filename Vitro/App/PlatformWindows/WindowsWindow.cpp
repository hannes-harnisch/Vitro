module;
#include "Core/Macros.hpp"
#include "Core/PlatformWindows/WindowsAPI.hpp"

#include <memory>
#include <string_view>
export module vt.App.Windows.Window;

import vt.App.AppContextBase;
import vt.Core.Rectangle;
import vt.Core.Vector;
import vt.Core.Windows.Utils;

namespace vt::windows
{
	export class WindowsWindow
	{
	public:
		static constexpr TCHAR const WINDOW_CLASS_NAME[] = TEXT(VT_ENGINE_NAME);
		static constexpr Rectangle	 DEFAULT_RECT {
			  .x	  = CW_USEDEFAULT,
			  .y	  = CW_USEDEFAULT,
			  .width  = static_cast<unsigned>(CW_USEDEFAULT),
			  .height = static_cast<unsigned>(CW_USEDEFAULT),
		  };

	protected:
		WindowsWindow(std::string_view title, Rectangle rect)
		{
			auto widened_title = widen_string(title);
			auto instance	   = AppContextBase::get().get_system_window_owner();

			HWND unowned_window = ::CreateWindowEx(WS_EX_APPWINDOW, WINDOW_CLASS_NAME, widened_title.data(),
												   WS_OVERLAPPEDWINDOW, rect.x, rect.y, rect.width, rect.height, nullptr,
												   nullptr, instance, nullptr);
			window.reset(unowned_window);
			VT_ENSURE(window != nullptr, "Failed to create window.");
		}

		void open()
		{
			::ShowWindow(window.get(), SW_RESTORE);
		}

		void close()
		{
			::ShowWindow(window.get(), SW_HIDE);
		}

		void maximize()
		{
			::ShowWindow(window.get(), SW_MAXIMIZE);
		}

		void minimize()
		{
			::ShowWindow(window.get(), SW_MINIMIZE);
		}

		void enable_cursor()
		{
			while(::ShowCursor(true) < 0)
			{}

			BOOL succeeded = ::ClipCursor(nullptr);
			VT_ASSERT(succeeded, "Failed to clip cursor.");
		}

		void disable_cursor()
		{
			while(::ShowCursor(false) >= 0)
			{}

			RECT rect;
			BOOL succeeded = ::GetClientRect(window.get(), &rect);
			VT_ASSERT(succeeded, "Failed to get window client rect.");

			debug_clear_error();
			::MapWindowPoints(window.get(), nullptr, reinterpret_cast<POINT*>(&rect), sizeof(RECT) / sizeof(POINT));
			VT_ASSERT(::GetLastError() == ERROR_SUCCESS, "Failed to map window points.");

			succeeded = ::ClipCursor(&rect);
			VT_ASSERT(succeeded, "Failed to clip cursor.");
		}

		Extent get_size() const
		{
			RECT rect;
			BOOL succeeded = ::GetWindowRect(window.get(), &rect);
			VT_ASSERT(succeeded, "Failed to get window rect.");
			return {
				.width	= static_cast<unsigned>(rect.right - rect.left),
				.height = static_cast<unsigned>(rect.bottom - rect.top),
			};
		}

		void set_size(Extent size)
		{
			BOOL succeeded = ::SetWindowPos(window.get(), nullptr, 0, 0, size.width, size.height, SWP_NOMOVE | SWP_NOZORDER);
			VT_ASSERT(succeeded, "Failed to set window position.");
		}

		Int2 get_position() const
		{
			RECT rect;
			BOOL succeeded = ::GetWindowRect(window.get(), &rect);
			VT_ASSERT(succeeded, "Failed to get window rect.");
			return {
				.x = rect.left,
				.y = rect.top,
			};
		}

		void set_position(Int2 position)
		{
			BOOL succeeded = ::SetWindowPos(window.get(), nullptr, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			VT_ASSERT(succeeded, "Failed to set window position.");
		}

		std::string get_title() const
		{
			debug_clear_error();
			int length = ::GetWindowTextLength(window.get());
			VT_ASSERT_PURE(::GetLastError() == ERROR_SUCCESS, "Failed to get window title length.");

			std::wstring title(length, L'\0');
			::GetWindowText(window.get(), title.data(), length + 1);
			VT_ASSERT_PURE(::GetLastError() == ERROR_SUCCESS, "Failed to get window title.");

			return narrow_string(title);
		}

		void set_title(std::string_view title)
		{
			auto widened_title = widen_string(title);

			BOOL succeeded = ::SetWindowText(window.get(), widened_title.data());
			VT_ASSERT(succeeded, "Failed to set window title.");
		}

		Rectangle client_area() const
		{
			RECT rect;
			BOOL succeeded = ::GetClientRect(window.get(), &rect);
			VT_ASSERT(succeeded, "Failed to get window client rect.");
			return {
				.x		= rect.left,
				.y		= rect.top,
				.width	= static_cast<unsigned>(rect.right - rect.left),
				.height = static_cast<unsigned>(rect.bottom - rect.top),
			};
		}

		HWND native_handle()
		{
			return window.get();
		}

	private:
		struct WindowDeleter
		{
			using pointer = HWND;
			void operator()(HWND hwnd)
			{
				BOOL succeeded = ::DestroyWindow(hwnd);
				VT_ASSERT(succeeded, "Failed to destroy window.");
			}
		};
		std::unique_ptr<HWND, WindowDeleter> window;

		static void debug_clear_error()
		{
#if VT_DEBUG
			::SetLastError(0);
#endif
		}
	};
}
