module;
#include "Core/Macros.hh"

#include <string_view>
#include <thread>
export module vt.App.Window;

import vt.App.AppContextBase;
import vt.Core.Rectangle;
import vt.Core.Vector;
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
			AppContextBase::get().notify_window_mapping(get_handle(), *this);
			GraphicsSystem::get().notify_window_construction(*this, get_handle());
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

		void open()
		{
			ensure_call_is_on_main_thread();
			Base::open();
		}

		void close()
		{
			ensure_call_is_on_main_thread();
			Base::close();
		}

		void maximize()
		{
			ensure_call_is_on_main_thread();
			Base::maximize();
		}

		void minimize()
		{
			ensure_call_is_on_main_thread();
			Base::minimize();
		}

		void enable_cursor()
		{
			ensure_call_is_on_main_thread();
			is_cursor_enabled = true;

			Base::enable_cursor();
		}

		void disable_cursor()
		{
			ensure_call_is_on_main_thread();
			is_cursor_enabled = false;

			Base::disable_cursor();
		}

		bool cursor_enabled() const
		{
			return is_cursor_enabled;
		}

		Extent get_size() const
		{
			ensure_call_is_on_main_thread();
			return Base::get_size();
		}

		void set_size(Extent size)
		{
			ensure_call_is_on_main_thread();
			Base::set_size(size);
		}

		Int2 get_position() const
		{
			ensure_call_is_on_main_thread();
			return Base::get_position();
		}

		void set_position(Int2 position)
		{
			ensure_call_is_on_main_thread();
			Base::set_position(position);
		}

		std::string get_title() const
		{
			ensure_call_is_on_main_thread();
			return Base::get_title();
		}

		void set_title(std::string_view title)
		{
			ensure_call_is_on_main_thread();
			Base::set_title(title);
		}

		Rectangle client_area() const
		{
			ensure_call_is_on_main_thread();
			return Base::client_area();
		}

		void* get_handle()
		{
			return Base::get_handle();
		}

	private:
		bool is_cursor_enabled = true;

		static void ensure_call_is_on_main_thread()
		{
			bool is_on_main_thread = AppContextBase::get().main_thread_id() == std::this_thread::get_id();
			VT_ENSURE(is_on_main_thread, "Window operations must happen on the main thread.");
		}

		void notify_destruction()
		{
			AppContextBase::get().notify_window_destruction(get_handle());
			GraphicsSystem::get().notify_window_destruction(*this);
		}

		void notify_move(Window& old_window)
		{
			AppContextBase::get().notify_window_mapping(get_handle(), *this);
			GraphicsSystem::get().notify_window_replacement(old_window, *this);
		}
	};
}
