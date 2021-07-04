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
		constexpr static Rectangle DefaultSize {uint32_t(CW_USEDEFAULT), uint32_t(CW_USEDEFAULT)};
		constexpr static int DefaultX = CW_USEDEFAULT;
		constexpr static int DefaultY = CW_USEDEFAULT;
		static inline Int2 const DefaultPosition {CW_USEDEFAULT, CW_USEDEFAULT}; // TODO ICE

		Window(std::string_view const title, Rectangle const size, Int2 const position) :
			windowHandle(makeWindowHandle(title, size, position))
		{}

		void open() final override
		{
			::ShowWindow(windowHandle, SW_RESTORE);
		}

		void close() final override
		{
			::ShowWindow(windowHandle, SW_HIDE);
		}

		void maximize() final override
		{
			::ShowWindow(windowHandle, SW_MAXIMIZE);
		}

		void minimize() final override
		{
			::ShowWindow(windowHandle, SW_MINIMIZE);
		}

		void enableCursor() final override
		{
			isCursorEnabled = true;
			while(::ShowCursor(true) < 0)
			{}

			::ClipCursor(nullptr);
		}

		void disableCursor() final override
		{
			isCursorEnabled = false;
			while(::ShowCursor(false) >= 0)
			{}

			RECT rect;
			::GetClientRect(windowHandle, &rect);
			::MapWindowPoints(windowHandle, nullptr, reinterpret_cast<POINT*>(&rect), sizeof(RECT) / sizeof(POINT));
			::ClipCursor(&rect);
		}

		Rectangle getViewport() const final override
		{
			RECT rect;
			::GetClientRect(windowHandle, &rect);
			return {static_cast<uint32_t>(rect.right), static_cast<uint32_t>(rect.bottom)};
		}

		Rectangle getSize() const final override
		{
			RECT rect;
			::GetWindowRect(windowHandle, &rect);
			LONG width	= rect.right - rect.left;
			LONG height = rect.bottom - rect.top;
			return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
		}

		void setSize(Rectangle const size) final override
		{
			::SetWindowPos(windowHandle, nullptr, 0, 0, size.width, size.height, SWP_NOMOVE | SWP_NOZORDER);
		}

		Int2 getPosition() const final override
		{
			RECT rect;
			::GetWindowRect(windowHandle, &rect);
			return {rect.left, rect.top};
		}

		void setPosition(Int2 const position) final override
		{
			::SetWindowPos(windowHandle, nullptr, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}

		std::string getTitle() const final override
		{
			int length = ::GetWindowTextLength(windowHandle);
			std::wstring title(length, L'\0');
			::GetWindowText(windowHandle, title.data(), length + 1);
			return narrowString(title);
		}

		void setTitle(std::string_view const title) final override
		{
			auto const widenedTitle = widenString(title);
			::SetWindowText(windowHandle, widenedTitle.data());
		}

		void* handle() final override
		{
			return windowHandle;
		}

	private:
		Unique<HWND, ::DestroyWindow> windowHandle;

		static decltype(windowHandle) makeWindowHandle(std::string_view const title, Rectangle const size, Int2 const position)
		{
			auto const widenedTitle	  = widenString(title);
			auto const instanceHandle = static_cast<HINSTANCE>(AppContextBase::get().handle());
			return ::CreateWindow(WindowClassName, widenedTitle.data(), WS_OVERLAPPEDWINDOW, position.x, position.y, size.width,
								  size.height, nullptr, nullptr, instanceHandle, nullptr);
		}
	};
}
