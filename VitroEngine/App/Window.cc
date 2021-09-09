module;
#include "Core/Macros.hh"

#include <string_view>
#include <thread>
export module vt.App.Window;

import vt.App.ObjectEvent;
import vt.Core.Rectangle;
import vt.Core.Vector;
import vt.VT_SYSTEM_MODULE.Window;

namespace vt
{
	using WindowBase = VT_SYSTEM_NAME::VT_PASTE(VT_SYSTEM_MODULE, Window);

	export class Window : public WindowBase, public ObjectEventSource<Window>
	{
	public:
		Window(std::string_view title, Rectangle rect = WindowBase::DefaultRect) : WindowBase(title, rect)
		{}

		void open()
		{
			WindowBase::open();
		}

		void close()
		{
			WindowBase::close();
		}

		void maximize()
		{
			WindowBase::maximize();
		}

		void minimize()
		{
			WindowBase::minimize();
		}

		void enable_cursor()
		{
			is_cursor_enabled = true;
			WindowBase::enable_cursor();
		}

		void disable_cursor()
		{
			is_cursor_enabled = false;
			WindowBase::disable_cursor();
		}

		bool cursor_enabled() const
		{
			return is_cursor_enabled;
		}

		Extent get_size() const
		{
			return WindowBase::get_size();
		}

		void set_size(Extent size)
		{
			WindowBase::set_size(size);
		}

		Int2 get_position() const
		{
			return WindowBase::get_position();
		}

		void set_position(Int2 position)
		{
			WindowBase::set_position(position);
		}

		std::string get_title() const
		{
			return WindowBase::get_title();
		}

		void set_title(std::string_view title)
		{
			WindowBase::set_title(title);
		}

		Rectangle client_area() const
		{
			return WindowBase::client_area();
		}

		auto native_handle()
		{
			return WindowBase::native_handle();
		}

	private:
		bool is_cursor_enabled = true;
	};
}
