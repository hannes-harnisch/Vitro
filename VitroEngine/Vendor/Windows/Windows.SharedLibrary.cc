module;
#include "Windows.API.hh"

#include <string_view>
export module Vitro.Windows.SharedLibrary;

import Vitro.App.SharedLibraryBase;
import Vitro.Core.Unique;
import Vitro.Windows.StringUtils;

namespace Windows
{
	export class SharedLibrary : public SharedLibraryBase
	{
	public:
		[[nodiscard]] bool reload() final override
		{
			libraryHandle.reset();
			libraryHandle = createLibraryHandle(name);
			return libraryHandle;
		}

	protected:
		static HMODULE createLibraryHandle(std::string_view name)
		{
			auto const widenedPath = widenString(name);
			return ::LoadLibrary(widenedPath.data());
		}

		SharedLibrary(HMODULE handle, std::string_view name) : libraryHandle(handle), name(name)
		{}

		void* loadSymbolAddress(std::string_view symbol) const final override
		{
			return ::GetProcAddress(libraryHandle, symbol.data());
		}

	private:
		Unique<HMODULE, ::FreeLibrary> libraryHandle;
		std::string name;
	};
}
