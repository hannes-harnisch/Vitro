module;
#include "Core/Macros.hh"
#include "Windows.API.hh"

#include <memory>
#include <string_view>
export module vt.Windows.Window;

import vt.Core.Rectangle;
import vt.Core.Vector;
import vt.Windows.Utils;

namespace vt::windows
{
	export class WindowsWindow
	{
	public:
		static constexpr wchar_t const WindowClassName[] = TEXT(VT_ENGINE_NAME);

	protected:
		static constexpr Rectangle DefaultRect {
			.x		= CW_USEDEFAULT,
			.y		= CW_USEDEFAULT,
			.width	= static_cast<unsigned>(CW_USEDEFAULT),
			.height = static_cast<unsigned>(CW_USEDEFAULT),
		};

		WindowsWindow(std::string_view title, Rectangle rect)
		{
			auto widened_title = widen_string(title);

			auto instance_handle = ::GetModuleHandleW(nullptr);
			HWND raw_window = ::CreateWindowExW(0, WindowClassName, widened_title.data(), WS_OVERLAPPEDWINDOW, rect.x, rect.y,
												rect.width, rect.height, nullptr, nullptr, instance_handle, nullptr);

			window.reset(raw_window);
			VT_ENSURE(raw_window, "Failed to create window.");
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

			::ClipCursor(nullptr);
		}

		void disable_cursor()
		{
			while(::ShowCursor(false) >= 0)
			{}

			RECT rect;
			::GetClientRect(window.get(), &rect);
			::MapWindowPoints(window.get(), nullptr, reinterpret_cast<POINT*>(&rect), sizeof(RECT) / sizeof(POINT));
			::ClipCursor(&rect);
		}

		Extent get_size() const
		{
			RECT rect;
			::GetWindowRect(window.get(), &rect);
			return {
				.width	= static_cast<unsigned>(rect.right - rect.left),
				.height = static_cast<unsigned>(rect.bottom - rect.top),
			};
		}

		void set_size(Extent size)
		{
			::SetWindowPos(window.get(), nullptr, 0, 0, size.width, size.height, SWP_NOMOVE | SWP_NOZORDER);
		}

		Int2 get_position() const
		{
			RECT rect;
			::GetWindowRect(window.get(), &rect);
			return {
				.x = rect.left,
				.y = rect.top,
			};
		}

		void set_position(Int2 position)
		{
			::SetWindowPos(window.get(), nullptr, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}

		std::string get_title() const
		{
			int length = ::GetWindowTextLengthW(window.get());

			std::wstring title(length, L'\0');
			::GetWindowTextW(window.get(), title.data(), length + 1);

			return narrow_string(title);
		}

		void set_title(std::string_view title)
		{
			auto widened_title = widen_string(title);
			::SetWindowTextW(window.get(), widened_title.data());
		}

		Rectangle client_area() const
		{
			RECT rect;
			::GetClientRect(window.get(), &rect);
			return {
				.x		= rect.left,
				.y		= rect.top,
				.width	= static_cast<unsigned>(rect.right - rect.left),
				.height = static_cast<unsigned>(rect.bottom - rect.top),
			};
		}

		HWND native_handle() const
		{
			return window.get();
		}

	private:
		struct WindowDeleter
		{
			using pointer = HWND;
			void operator()(HWND hwnd) const
			{
				::DestroyWindow(hwnd);
			}
		};
		std::unique_ptr<HWND, WindowDeleter> window;
	};
}
