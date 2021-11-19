module;
#include <atomic>
#include <string>
module vt.App.AppSystem;

import vt.App.AppContext;
import vt.App.EventSystem;
import vt.App.Input;
import vt.Core.Version;

namespace vt
{
	AppSystem::AppSystem(std::atomic_bool& engine_running_status) : app_context(engine_running_status)
	{}

	void AppSystem::pump_system_events()
	{
		app_context.pump_system_events();
	}

	std::string AppSystem::get_current_app_name() const
	{
		return "VitroEditor";
	}

	Version AppSystem::get_current_app_version() const
	{
		return {0, 0, 1};
	}
}
