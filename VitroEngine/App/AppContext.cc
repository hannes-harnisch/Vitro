module;
#include "Core/Macros.hh"

#include <atomic>
#include <unordered_map>
#include <vector>
export module vt.App.AppContext;

import vt.App.EventListener;
import vt.App.ObjectEvent;
import vt.App.Window;
import vt.VT_SYSTEM_MODULE.AppContext;

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

		void poll_events() const
		{
			SystemAppContext::poll_events();
		}

	private:
		std::atomic_bool&	 engine_running_status;
		std::vector<Window*> open_windows;

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

		void on_window_object_construct(ObjectConstructEvent<Window>& event)
		{
			system_window_to_engine_window.try_emplace(event.object.native_handle(), &event.object);
		}

		void on_window_object_move_construct(ObjectMoveConstructEvent<Window>& event)
		{
			system_window_to_engine_window[event.constructed.native_handle()] = &event.constructed;
		}

		void on_window_object_destroy(ObjectDestroyEvent<Window>& event)
		{
			system_window_to_engine_window.erase(event.object.native_handle());
		}

		void on_window_object_move_assign(ObjectMoveAssignEvent<Window>& event)
		{
			std::erase_if(system_window_to_engine_window, [&](auto const& pair) {
				return pair.second == &event.left;
			});
			system_window_to_engine_window.try_emplace(event.left.native_handle(), &event.left);
		}
	};
}
