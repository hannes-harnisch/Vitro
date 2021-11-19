module;
#include "VitroCore/Macros.hpp"
#include VT_SYSTEM_HEADER

#include <string_view>
module vt.App.Window;

import vt.App.ObjectEvent;
import vt.App.VT_SYSTEM_MODULE.Window;
import vt.Core.Rect;
import vt.Core.Vector;

namespace vt
{
	Window::Window(std::string_view title, Rectangle rect) : SystemWindow(title, rect)
	{}

	void Window::open()
	{
		SystemWindow::open();
	}

	void Window::close()
	{
		SystemWindow::close();
	}

	void Window::maximize()
	{
		SystemWindow::maximize();
	}

	void Window::minimize()
	{
		SystemWindow::minimize();
	}

	void Window::enable_cursor()
	{
		is_cursor_enabled = true;
		SystemWindow::enable_cursor();
	}

	void Window::disable_cursor()
	{
		is_cursor_enabled = false;
		SystemWindow::disable_cursor();
	}

	bool Window::cursor_enabled() const
	{
		return is_cursor_enabled;
	}

	void Window::enable_resize()
	{
		is_resize_enabled = true;
		SystemWindow::enable_resize();
	}

	void Window::disable_resize()
	{
		is_resize_enabled = false;
		SystemWindow::disable_resize();
	}

	bool Window::resize_enabled() const
	{
		return is_resize_enabled;
	}

	Extent Window::get_size() const
	{
		return SystemWindow::get_size();
	}

	void Window::set_size(Extent size)
	{
		SystemWindow::set_size(size);
	}

	Int2 Window::get_position() const
	{
		return SystemWindow::get_position();
	}

	void Window::set_position(Int2 position)
	{
		SystemWindow::set_position(position);
	}

	std::string Window::get_title() const
	{
		return SystemWindow::get_title();
	}

	void Window::set_title(std::string_view title)
	{
		SystemWindow::set_title(title);
	}

	Rectangle Window::client_area() const
	{
		return SystemWindow::client_area();
	}

	SystemWindowHandle Window::native_handle()
	{
		return SystemWindow::native_handle();
	}
}
