module;
#include "VitroCore/Macros.hpp"
#include VT_SYSTEM_HEADER
module vt.App.AppContextBase;

namespace vt
{
	SystemWindowOwnerHandle AppContextBase::get_system_window_owner()
	{
		return get().window_owner_handle;
	}

	AppContextBase::AppContextBase(SystemWindowOwnerHandle window_owner) : window_owner_handle(window_owner)
	{}

	Window* AppContextBase::find_window(SystemWindowHandle handle) const
	{
		auto it = system_window_to_engine_window.find(handle);

		if(it == system_window_to_engine_window.end())
			return nullptr;

		return it->second;
	}
}
