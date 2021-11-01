module;
#include "Core/Macros.hpp"
#include "Core/PlatformWindows/WindowsAPI.hpp"

#include <memory>
#include <string_view>
export module vt.App.Windows.Window;

import vt.App.AppContextBase;
import vt.Core.Rect;
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
		WindowsWindow(std::string_view title, Rectangle rect) : window(make_window(title, rect))
		{}

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

			call_win32<::ClipCursor>("Failed to clip cursor.", nullptr);
		}

		void disable_cursor()
		{
			while(::ShowCursor(false) >= 0)
			{}

			RECT rect;
			call_win32<::GetClientRect>("Failed to get window client rect.", window.get(), &rect);

			call_win32<::MapWindowPoints>("Failed to map window points.", window.get(), nullptr,
										  reinterpret_cast<POINT*>(&rect), static_cast<UINT>(sizeof(RECT) / sizeof(POINT)));

			call_win32<::ClipCursor>("Failed to clip cursor.", &rect);
		}

		void enable_resize()
		{
			make_resizable(true);
		}

		void disable_resize()
		{
			make_resizable(false);
		}

		Extent get_size() const
		{
			RECT rect;
			call_win32<::GetWindowRect>("Failed to get window rect.", window.get(), &rect);
			return {
				.width	= static_cast<unsigned>(rect.right - rect.left),
				.height = static_cast<unsigned>(rect.bottom - rect.top),
			};
		}

		void set_size(Extent size)
		{
			call_win32<::SetWindowPos>("Failed to set window position.", window.get(), nullptr, 0, 0, size.width, size.height,
									   SWP_NOMOVE | SWP_NOZORDER);
		}

		Int2 get_position() const
		{
			RECT rect;
			call_win32<::GetWindowRect>("Failed to get window rect.", window.get(), &rect);
			return {
				.x = rect.left,
				.y = rect.top,
			};
		}

		void set_position(Int2 position)
		{
			call_win32<::SetWindowPos>("Failed to set window position.", window.get(), nullptr, position.x, position.y, 0, 0,
									   SWP_NOSIZE | SWP_NOZORDER);
		}

		std::string get_title() const
		{
			int length = call_win32<::GetWindowTextLength>("Failed to get window title length.", window.get());

			std::wstring title(length, L'\0');
			call_win32<::GetWindowText>("Failed to get window title.", window.get(), title.data(), length + 1);

			return narrow_string(title);
		}

		void set_title(std::string_view title)
		{
			auto widened_title = widen_string(title);
			call_win32<::SetWindowText>("Failed to set window title.", window.get(), widened_title.data());
		}

		Rectangle client_area() const
		{
			RECT rect;
			call_win32<::GetClientRect>("Failed to get window client rect.", window.get(), &rect);
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
			void operator()(HWND hwnd) const
			{
				call_win32<::DestroyWindow>("Failed to destroy window.", hwnd);
			}
		};
		std::unique_ptr<HWND, WindowDeleter> window;

		static HWND make_window(std::string_view title, Rectangle rect)
		{
			auto widened_title = widen_string(title);
			auto instance	   = AppContextBase::get_system_window_owner();

			return call_win32<::CreateWindowEx>("Failed to create window.", WS_EX_APPWINDOW, WINDOW_CLASS_NAME,
												widened_title.data(), WS_OVERLAPPEDWINDOW, rect.x, rect.y, rect.width,
												rect.height, nullptr, nullptr, instance, nullptr);
		}

		void make_resizable(bool enable) const
		{
			auto old_style = call_win32<::GetWindowLongPtr>("Failed to get Windows window attributes.", window.get(),
															GWL_STYLE);

			LONG_PTR new_style;
			if(enable)
				new_style = old_style | WS_THICKFRAME;
			else
				new_style = old_style ^ WS_THICKFRAME;

			call_win32<::SetWindowLongPtr>("Failed to set Windows window attributes.", window.get(), GWL_STYLE, new_style);
		}
	};
}
