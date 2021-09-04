module;
#include <atomic>
#include <vector>
export module vt.App.AppContext;

import vt.App.EventListener;
import vt.App.KeyCode;
import vt.App.Window;
import vt.App.WindowEvent;
import vt.VT_SYSTEM_MODULE.AppContext;

namespace vt
{
	export class AppContext : public VT_SYSTEM_NAME::AppContext, public EventListener
	{
		friend class AppSystem;

		std::atomic_bool&	 engine_running_status;
		std::vector<Window*> open_windows;

		AppContext(std::atomic_bool& engine_running_status) : engine_running_status(engine_running_status)
		{
			register_event_handlers<&AppContext::on_window_open, &AppContext::on_window_close, &AppContext::on_escape_held>();
		}

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
	};
}
