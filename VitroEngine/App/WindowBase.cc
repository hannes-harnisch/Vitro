module;
#include "Core/Macros.hh"

#include <string>
#include <thread>
export module vt.App.WindowBase;

import vt.App.AppContextBase;
import vt.Core.Rectangle;
import vt.Core.Vector;

namespace vt
{
	export class WindowBase
	{
	public:
		virtual void		open()							  = 0;
		virtual void		close()							  = 0;
		virtual void		maximize()						  = 0;
		virtual void		minimize()						  = 0;
		virtual void		enable_cursor()					  = 0;
		virtual void		disable_cursor()				  = 0;
		virtual Extent		get_size() const				  = 0;
		virtual void		set_size(Extent size)			  = 0;
		virtual Int2		get_position() const			  = 0;
		virtual void		set_position(Int2 position)		  = 0;
		virtual std::string get_title() const				  = 0;
		virtual void		set_title(std::string_view title) = 0;
		virtual Rectangle	client_area() const				  = 0;
		virtual void*		handle()						  = 0;

		bool cursor_enabled() const
		{
			return is_cursor_enabled;
		}

	protected:
		bool is_cursor_enabled = true;

		static void ensure_call_is_on_main_thread()
		{
			bool is_on_main_thread = AppContextBase::get().main_thread_id() == std::this_thread::get_id();
			VT_ENSURE(is_on_main_thread, "Window operations must happen on the main thread.");
		}
	};
}
