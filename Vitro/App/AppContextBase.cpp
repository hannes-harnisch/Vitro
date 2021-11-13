module;
#include "VitroCore/Macros.hpp"
#include VT_SYSTEM_HEADER

#include <unordered_map>
export module vt.App.AppContextBase;

import vt.Core.Singleton;

namespace vt
{
	// Provides state shared between all platform-specific app context implementations. Specifically it stores the mapping
	// between the platform-specific window handles and what engine window it corresponds to.
	export class AppContextBase : public Singleton<AppContextBase>
	{
	public:
		static SystemWindowOwnerHandle get_system_window_owner()
		{
			return get().window_owner_handle;
		}

	protected:
		std::unordered_map<SystemWindowHandle, class Window*> system_window_to_engine_window;

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
