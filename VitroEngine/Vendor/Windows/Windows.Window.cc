module;
#include "Windows.API.hh"

#include <string_view>
export module Vitro.Windows.Window;

import Vitro.App.AppContextBase;
import Vitro.App.WindowBase;
import Vitro.Core.Unique;
import Vitro.Math.Rectangle;
import Vitro.Math.Vector;
import Vitro.Windows.StringUtils;

namespace vt::windows
{
	export class Window : public WindowBase
	{
	public:
		constexpr static auto WindowClassName = TEXT(VT_ENGINE_NAME);
		constexpr static Rectangle DefaultSize {unsigned(CW_USEDEFAULT), unsigned(CW_USEDEFAULT)};
		constexpr static int DefaultX = CW_USEDEFAULT;
		constexpr static int DefaultY = CW_USEDEFAULT;
		static inline Int2 const DefaultPosition {CW_USEDEFAULT, CW_USEDEFAULT}; // TODO ICE

		Window(std::string_view const title, Rectangle const size, Int2 const position) :
			window(makeWindow(title, size, position))
		{}

		void open() final override
		{
			ensureCallIsOnMainThread();
			::ShowWindow(window, SW_RESTORE);
		}

		void close() final override
		{
			ensureCallIsOnMainThread();
			::ShowWindow(window, SW_HIDE);
		}

		void maximize() final override
		{
			ensureCallIsOnMainThread();
			::ShowWindow(window, SW_MAXIMIZE);
		}

		void minimize() final override
		{
			ensureCallIsOnMainThread();
			::ShowWindow(window, SW_MINIMIZE);
		}

		void enableCursor() final override
		{
			ensureCallIsOnMainThread();

			isCursorEnabled = true;
			while(::ShowCursor(true) < 0)
			{}

			::ClipCursor(nullptr);
		}

		void disableCursor() final override
		{
			ensureCallIsOnMainThread();

			isCursorEnabled = false;
			while(::ShowCursor(false) >= 0)
			{}

			RECT rect;
			::GetClientRect(window, &rect);
			::MapWindowPoints(window, nullptr, reinterpret_cast<POINT*>(&rect), sizeof(RECT) / sizeof(POINT));
			::ClipCursor(&rect);
		}

		Rectangle getSize() const final override
		{
			ensureCallIsOnMainThread();

			RECT rect;
			::GetWindowRect(window, &rect);

			LONG width	= rect.right - rect.left;
			LONG height = rect.bottom - rect.top;
			return {static_cast<unsigned>(width), static_cast<unsigned>(height)};
		}

		void setSize(Rectangle const size) final override
		{
			ensureCallIsOnMainThread();
			::SetWindowPos(window, nullptr, 0, 0, size.width, size.height, SWP_NOMOVE | SWP_NOZORDER);
		}

		Int2 getPosition() const final override
		{
			ensureCallIsOnMainThread();

			RECT rect;
			::GetWindowRect(window, &rect);
			return {rect.left, rect.top};
		}

		void setPosition(Int2 const position) final override
		{
			ensureCallIsOnMainThread();
			::SetWindowPos(window, nullptr, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}

		std::string getTitle() const final override
		{
			ensureCallIsOnMainThread();

			int length = ::GetWindowTextLength(window);
			std::wstring title(length, L'\0');
			::GetWindowText(window, title.data(), length + 1);
			return narrowString(title);
		}

		void setTitle(std::string_view const title) final override
		{
			ensureCallIsOnMainThread();

			auto const widenedTitle = widenString(title);
			::SetWindowText(window, widenedTitle.data());
		}

		Rectangle viewport() const final override
		{
			ensureCallIsOnMainThread();

			RECT rect;
			::GetClientRect(window, &rect);
			return {static_cast<unsigned>(rect.right), static_cast<unsigned>(rect.bottom)};
		}

		void* handle() final override
		{
			return window;
		}

	private:
		Unique<HWND, ::DestroyWindow> window;

		static Unique<HWND, ::DestroyWindow> makeWindow(std::string_view const title, Rectangle const size, Int2 const position)
		{
			auto const widenedTitle	  = widenString(title);
			auto const instanceHandle = static_cast<HINSTANCE>(AppContextBase::get().handle());
			return ::CreateWindow(WindowClassName, widenedTitle.data(), WS_OVERLAPPEDWINDOW, position.x, position.y, size.width,
								  size.height, nullptr, nullptr, instanceHandle, nullptr);
		}
	};
}
