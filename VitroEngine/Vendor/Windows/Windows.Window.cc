module;
#include "Core/Macros.hh"
#include "Windows.API.hh"

#include <string_view>
export module Vitro.Windows.Window;

import Vitro.App.AppContextBase;
import Vitro.App.WindowBase;
import Vitro.Core.Rectangle;
import Vitro.Core.Unique;
import Vitro.Core.Vector;
import Vitro.Windows.Utils;

namespace vt::windows
{
	export class Window : public WindowBase
	{
	public:
		static constexpr wchar_t const WindowClassName[] = TEXT(VT_ENGINE_NAME);

		static constexpr Rectangle DefaultRect {
			.x		= CW_USEDEFAULT,
			.y		= CW_USEDEFAULT,
			.width	= static_cast<unsigned>(CW_USEDEFAULT),
			.height = static_cast<unsigned>(CW_USEDEFAULT),
		};

		Window(std::string_view title, Rectangle rect) : window(makeWindow(title, rect))
		{}

		void open() final override
		{
			ensureCallIsOnMainThread();
			::ShowWindow(window.get(), SW_RESTORE);
		}

		void close() final override
		{
			ensureCallIsOnMainThread();
			::ShowWindow(window.get(), SW_HIDE);
		}

		void maximize() final override
		{
			ensureCallIsOnMainThread();
			::ShowWindow(window.get(), SW_MAXIMIZE);
		}

		void minimize() final override
		{
			ensureCallIsOnMainThread();
			::ShowWindow(window.get(), SW_MINIMIZE);
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
			::GetClientRect(window.get(), &rect);
			::MapWindowPoints(window.get(), nullptr, reinterpret_cast<POINT*>(&rect), sizeof(RECT) / sizeof(POINT));
			::ClipCursor(&rect);
		}

		Extent getSize() const final override
		{
			ensureCallIsOnMainThread();

			RECT rect;
			::GetWindowRect(window.get(), &rect);
			return {
				.width	= static_cast<unsigned>(rect.right - rect.left),
				.height = static_cast<unsigned>(rect.bottom - rect.top),
			};
		}

		void setSize(Extent size) final override
		{
			ensureCallIsOnMainThread();
			::SetWindowPos(window.get(), nullptr, 0, 0, size.width, size.height, SWP_NOMOVE | SWP_NOZORDER);
		}

		Int2 getPosition() const final override
		{
			ensureCallIsOnMainThread();

			RECT rect;
			::GetWindowRect(window.get(), &rect);
			return {
				.x = rect.left,
				.y = rect.top,
			};
		}

		void setPosition(Int2 position) final override
		{
			ensureCallIsOnMainThread();
			::SetWindowPos(window.get(), nullptr, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}

		std::string getTitle() const final override
		{
			ensureCallIsOnMainThread();

			int length = ::GetWindowTextLengthW(window.get());

			std::wstring title(length, L'\0');
			::GetWindowTextW(window.get(), title.data(), length + 1);

			return narrowString(title);
		}

		void setTitle(std::string_view const title) final override
		{
			ensureCallIsOnMainThread();

			auto widenedTitle = widenString(title);
			::SetWindowTextW(window.get(), widenedTitle.data());
		}

		Rectangle clientArea() const final override
		{
			ensureCallIsOnMainThread();

			RECT rect;
			::GetClientRect(window.get(), &rect);
			return {
				.x		= rect.left,
				.y		= rect.top,
				.width	= static_cast<unsigned>(rect.right - rect.left),
				.height = static_cast<unsigned>(rect.bottom - rect.top),
			};
		}

		void* handle() final override
		{
			return window.get();
		}

	private:
		Unique<HWND, ::DestroyWindow> window;

		static decltype(window) makeWindow(std::string_view title, Rectangle rect)
		{
			auto widenedTitle	= widenString(title);
			auto instanceHandle = static_cast<HINSTANCE>(AppContextBase::get().handle());

			HWND rawWindow = ::CreateWindowExW(0, WindowClassName, widenedTitle.data(), WS_OVERLAPPEDWINDOW, rect.x, rect.y,
											   rect.width, rect.height, nullptr, nullptr, instanceHandle, nullptr);
			decltype(window) freshWindow(rawWindow);
			vtEnsure(rawWindow, "Failed to create window.");

			return freshWindow;
		}
	};
}
