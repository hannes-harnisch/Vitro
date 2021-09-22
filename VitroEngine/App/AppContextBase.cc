module;
#include "Core/Macros.hh"
#include VT_SYSTEM_HEADER

#include <unordered_map>
export module vt.App.AppContextBase;

import vt.Core.Singleton;

namespace vt
{
	export class AppContextBase : public Singleton<AppContextBase>
	{
	public:
		SystemWindowOwnerHandle get_system_window_owner() const
		{
			return window_owner_handle;
		}

	protected:
		std::unordered_map<SystemWindowHandle, Window*> system_window_to_engine_window;

		AppContextBase(SystemWindowOwnerHandle window_owner) : window_owner_handle(window_owner)
		{}

		~AppContextBase() = default;

		Window* find_window(SystemWindowHandle handle) const
		{
			auto it = system_window_to_engine_window.find(handle);

			if(it == system_window_to_engine_window.end())
				return nullptr;

			return it->second;
		}

	private:
		SystemWindowOwnerHandle window_owner_handle;
	};
}
