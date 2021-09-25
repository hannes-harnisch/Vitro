module;
#include "Core/Macros.hpp"

#include <string_view>
export module vt.App.Window;

import vt.App.ObjectEvent;
import vt.App.VT_SYSTEM_MODULE.Window;
import vt.Core.Rectangle;
import vt.Core.Vector;

namespace vt
{
	using SystemWindow = VT_SYSTEM_NAME::VT_PASTE(VT_SYSTEM_MODULE, Window);

	class WindowImpl : private SystemWindow
	{
	public:
		WindowImpl(std::string_view title, Rectangle rect = SystemWindow::DefaultRect) : SystemWindow(title, rect)
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

	protected:
		~WindowImpl() = default;

	private:
		bool is_cursor_enabled = true;
	};

	export class Window : public WindowImpl, public ObjectEventSentinel<Window>
	{
		using WindowImpl::WindowImpl;
	};
}
