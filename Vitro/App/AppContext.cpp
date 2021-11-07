module;
#include "Core/Macros.hpp"

#include <atomic>
#include <unordered_map>
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
		AppContext(std::atomic_bool& engine_running_status) : engine_running_status(engine_running_status)
		{
			register_event_handlers<&AppContext::on_window_open, &AppContext::on_window_close, &AppContext::on_escape_held,
									&AppContext::on_window_object_construct, &AppContext::on_window_object_move_construct,
									&AppContext::on_window_object_destroy, &AppContext::on_window_object_move_assign>();
		}

		void pump_system_events()
		{
			SystemAppContext::pump_system_events();
		}

	private:
		std::atomic_bool&  engine_running_status;
		SmallList<Window*> open_windows;

		void on_window_open(WindowOpenEvent& event)
		{
			open_windows.emplace_back(&event.window);
		}

		void on_window_close(WindowCloseEvent& event)
		{
			std::erase(open_windows, &event.window);
			if(open_windows.empty())
				engine_running_status = false;
		}

		void on_escape_held(KeyDownEvent& event)
		{
			if(event.key == KeyCode::Escape && event.repeats == 10)
				event.window.close();
		}

		void on_window_object_construct(ObjectConstructEvent<Window>& window_constructed)
		{
			auto& window = window_constructed.object;
			system_window_to_engine_window.try_emplace(window.native_handle(), &window);
		}

		void on_window_object_move_construct(ObjectMoveConstructEvent<Window>& window_moved)
		{
			auto& window = window_moved.constructed;

			system_window_to_engine_window[window.native_handle()] = &window;
		}

		void on_window_object_destroy(ObjectDestroyEvent<Window>& window_destroyed)
		{
			auto handle = window_destroyed.object.native_handle();
			system_window_to_engine_window.erase(handle);
		}

		void on_window_object_move_assign(ObjectMoveAssignEvent<Window>& window_moved)
		{
			auto& window = window_moved.left;
			std::erase_if(system_window_to_engine_window, [&](auto const& pair) {
				return pair.second == &window;
			});
			system_window_to_engine_window.try_emplace(window.native_handle(), &window);
		}
	};
}
