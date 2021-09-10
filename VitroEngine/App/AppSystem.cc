module;
#include <atomic>
export module vt.App.AppSystem;

import vt.App.AppContext;
import vt.App.EventSystem;
import vt.App.Input;

namespace vt
{
	export class AppSystem
	{
	public:
		AppSystem(std::atomic_bool& engine_running_status) : app_context(engine_running_status)
		{}

		void update()
		{
			app_context.poll_events();
		}

	private:
		EventSystem event_system;
		AppContext	app_context;
		Input		input;
	};
}
