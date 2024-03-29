module;
#include "VitroCore/Macros.hpp"

#include <string_view>
export module vt.App.Window;

import vt.App.ObjectEvent;
import vt.App.VT_SYSTEM_MODULE.Window;
import vt.Core.Rect;
import vt.Core.Vector;

namespace vt
{
	using SystemWindow = VT_SYSTEM_NAME::VT_PASTE(VT_SYSTEM_MODULE, Window);

	class WindowBaseData
	{
	protected:
		bool is_cursor_enabled = true;
		bool is_resize_enabled = true;
	};

	export class Window : private SystemWindow, private WindowBaseData, public ObjectEventSentinel<Window>
	{
	public:
		Window(std::string_view title, Rectangle rect = SystemWindow::DEFAULT_RECT) : SystemWindow(title, rect)
		{}

		void open()
		{
			SystemWindow::open();
		}

		void close()
		{
			SystemWindow::close();
		}

		void maximize()
		{
			SystemWindow::maximize();
		}

		void minimize()
		{
			SystemWindow::minimize();
		}

		void enable_cursor()
		{
			is_cursor_enabled = true;
			SystemWindow::enable_cursor();
		}

		void disable_cursor()
		{
			is_cursor_enabled = false;
			SystemWindow::disable_cursor();
		}

		bool cursor_enabled() const
		{
			return is_cursor_enabled;
		}

		void enable_resize()
		{
			is_resize_enabled = true;
			SystemWindow::enable_resize();
		}

		void disable_resize()
		{
			is_resize_enabled = false;
			SystemWindow::disable_resize();
		}

		bool resize_enabled() const
		{
			return is_resize_enabled;
		}

		Extent get_size() const
		{
			return SystemWindow::get_size();
		}

		void set_size(Extent size)
		{
			SystemWindow::set_size(size);
		}

		Int2 get_position() const
		{
			return SystemWindow::get_position();
		}

		void set_position(Int2 position)
		{
			SystemWindow::set_position(position);
		}

		std::string get_title() const
		{
			return SystemWindow::get_title();
		}

		void set_title(std::string_view title)
		{
			SystemWindow::set_title(title);
		}

		Rectangle client_area() const
		{
			return SystemWindow::client_area();
		}

		auto native_handle()
		{
			return SystemWindow::native_handle();
		}
	};
}
