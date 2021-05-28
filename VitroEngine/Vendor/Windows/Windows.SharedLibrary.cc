module;
#include "Windows.API.hh"

#include <optional>
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
		static std::optional<SharedLibrary> from(std::string_view name)
		{
			auto handle = createLibraryHandle(name);
			if(handle)
				return SharedLibrary(handle, name);
			else
				return {};
		}

		[[nodiscard]] bool reload() override
		{
			libraryHandle.reset();
			libraryHandle = createLibraryHandle(name);
			return libraryHandle;
		}

	protected:
		void* loadSymbolAddress(std::string_view symbol) const override
		{
			return ::GetProcAddress(libraryHandle, symbol.data());
		}

	private:
		Unique<HMODULE, ::FreeLibrary> libraryHandle;
		std::string name;

		static HMODULE createLibraryHandle(std::string_view name)
		{
			const auto widenedPath = widenString(name);
			return ::LoadLibrary(widenedPath.data());
		}

		SharedLibrary(HMODULE handle, std::string_view name) : libraryHandle(handle), name(name)
		{}
	};
}
