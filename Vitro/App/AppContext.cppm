module;
#include "VitroCore/Macros.hpp"

#include <atomic>
export module vt.App.AppContext;

import vt.App.EventListener;
import vt.App.ObjectEvent;
import vt.App.Window;
import vt.App.VT_SYSTEM_MODULE.AppContext;
import vt.Core.SmallList;

namespace vt
{
	using SystemAppContext = VT_SYSTEM_NAME::VT_PASTE(VT_SYSTEM_MODULE, AppContext);

	export class AppContext : private SystemAppContext, public EventListener
	{
	public:
		AppContext(std::atomic_bool& engine_running_status);

		void pump_system_events();

	private:
		std::atomic_bool&  engine_running_status;
		SmallList<Window*> open_windows;

		void on_window_open(WindowOpenEvent& event);
		void on_window_close(WindowCloseEvent& event);
		void on_escape_held(KeyDownEvent& event);
		void on_window_object_construct(ObjectConstructEvent<Window>& window_constructed);
		void on_window_object_move_construct(ObjectMoveConstructEvent<Window>& window_moved);
		void on_window_object_destroy(ObjectDestroyEvent<Window>& window_destroyed);
		void on_window_object_move_assign(ObjectMoveAssignEvent<Window>& window_moved);
	};
}
