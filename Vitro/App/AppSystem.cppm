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
		AppSystem(std::atomic_bool& engine_running_status);

		void		pump_system_events();
		std::string get_current_app_name() const;
		Version		get_current_app_version() const;

	private:
		EventSystem event_system;
		AppContext	app_context;
		Input		input;
	};
}
