module;
#include "VitroCore/Macros.hpp"
#include VT_SYSTEM_HEADER

#include <string_view>
export module vt.App.Window;

import vt.App.ObjectEvent;
import vt.App.VT_SYSTEM_MODULE.Window;
import vt.Core.Rect;
import vt.Core.Vector;

namespace vt
{
	using SystemWindow = VT_SYSTEM_NAME::VT_PASTE(VT_SYSTEM_MODULE, Window);

	class WindowBase
	{
	protected:
		bool is_cursor_enabled = true;
		bool is_resize_enabled = true;
	};

	export class Window : private SystemWindow, private WindowBase, public ObjectEventSentinel<Window>
	{
	public:
		Window(std::string_view title, Rectangle rect = SystemWindow::DEFAULT_RECT);

		void			   open();
		void			   close();
		void			   maximize();
		void			   minimize();
		void			   enable_cursor();
		void			   disable_cursor();
		bool			   cursor_enabled() const;
		void			   enable_resize();
		void			   disable_resize();
		bool			   resize_enabled() const;
		Extent			   get_size() const;
		void			   set_size(Extent size);
		Int2			   get_position() const;
		void			   set_position(Int2 position);
		std::string		   get_title() const;
		void			   set_title(std::string_view title);
		Rectangle		   client_area() const;
		SystemWindowHandle native_handle();
	};
}
