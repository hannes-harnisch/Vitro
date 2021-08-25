module;
#include <string_view>
export module vt.App.Window;

import vt.App.AppContextBase;
import vt.Core.Rectangle;
import vt.Graphics.GraphicsSystem;
import vt.VT_SYSTEM_MODULE.Window;

namespace vt
{
	export class Window final : public VT_SYSTEM_NAME::Window
	{
		using Base = VT_SYSTEM_NAME::Window;

	public:
		Window(std::string_view title, Rectangle rect = Base::DefaultRect) : Base(title, rect)
		{
			ensure_call_is_on_main_thread();
			AppContextBase::get().notify_window_mapping(handle(), *this);
			GraphicsSystem::get().notify_window_construction(*this, handle());
		}

		Window(Window&& other) noexcept(false) : Base(std::move(other))
		{
			ensure_call_is_on_main_thread();
			notify_move(other);
		}

		~Window()
		{
			ensure_call_is_on_main_thread();
			notify_destruction();
		}

		Window& operator=(Window&& other) noexcept(false)
		{
			ensure_call_is_on_main_thread();

			notify_destruction();
			Base::operator=(std::move(other));
			notify_move(other);
			return *this;
		}

	private:
		void notify_destruction()
		{
			AppContextBase::get().notify_window_destruction(handle());
			GraphicsSystem::get().notify_window_destruction(*this);
		}

		void notify_move(Window& old_window)
		{
			AppContextBase::get().notify_window_mapping(handle(), *this);
			GraphicsSystem::get().notify_window_replacement(old_window, *this);
		}
	};
}
