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

namespace Windows
{
	export class Window : public WindowBase
	{
	public:
		constexpr static auto WindowClassName = TEXT(VE_ENGINE_NAME);
		constexpr static Rectangle DefaultSize {uint32_t(CW_USEDEFAULT), uint32_t(CW_USEDEFAULT)};
		constexpr static int DefaultX = CW_USEDEFAULT;
		constexpr static int DefaultY = CW_USEDEFAULT;
		const static inline Int2 DefaultPosition {CW_USEDEFAULT, CW_USEDEFAULT}; // TODO: constexpr after compiler fix

		Window(const std::string_view title, const Rectangle size, const Int2 position) :
			windowHandle(createWindowHandle(title, size, position))
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
			::CloseWindow(windowHandle);
		}

		Rectangle getViewport() const final override
		{
			RECT rect;
			::GetClientRect(windowHandle, &rect);
			return {uint32_t(rect.right), uint32_t(rect.bottom)};
		}

		void* handle() final override
		{
			return windowHandle;
		}

	private:
		Unique<HWND, ::DestroyWindow> windowHandle;

		static HWND createWindowHandle(const std::string_view title, const Rectangle size, const Int2 position)
		{
			const auto widenedTitle	  = widenString(title);
			const auto instanceHandle = static_cast<HINSTANCE>(AppContextBase::get().handle());
			return ::CreateWindow(WindowClassName, widenedTitle.data(), WS_OVERLAPPEDWINDOW, position.x, position.y, size.width,
								  size.height, nullptr, nullptr, instanceHandle, nullptr);
		}
	};
}
