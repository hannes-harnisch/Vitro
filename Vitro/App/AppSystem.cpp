module;
#include <atomic>
#include <string>
export module vt.App.AppSystem;

import vt.App.AppContext;
import vt.App.EventSystem;
import vt.App.Input;
import vt.Core.Version;

namespace vt
{
	export class AppSystem
	{
	public:
		AppSystem(std::atomic_bool& engine_running_status) : app_context(engine_running_status)
		{}

		void pump_system_events()
		{
			app_context.pump_system_events();
		}

		std::string get_current_app_name() const
		{
			return "VitroEditor";
		}

		Version get_current_app_version() const
		{
			return {0, 0, 1};
		}

	private:
		EventSystem event_system;
		AppContext	app_context;
		Input		input;
	};
}
