module;
#include <atomic>
#include <vector>
export module vt.App.AppSystem;

import vt.App.AppContext;
import vt.App.EventListener;
import vt.App.Input;
import vt.App.Window;
import vt.App.WindowEvent;

namespace vt
{
	export class AppSystem : public EventListener
	{
	public:
		AppSystem(std::atomic_bool& engine_running_status) : engine_running_status(engine_running_status)
		{
			register_event_handlers<&AppSystem::on_window_open, &AppSystem::on_window_close, &AppSystem::on_escape_held>();
		}

		void update()
		{
			app_context.poll_events();
		}

	private:
		std::atomic_bool&	 engine_running_status;
		std::vector<Window*> open_windows;
		AppContext			 app_context;
		Input				 input;

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
