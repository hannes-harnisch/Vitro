module;
#include "Windows.API.hh"

#include <string_view>
export module Vitro.Windows.SharedLibrary;

import Vitro.App.SharedLibraryBase;
import Vitro.Core.Unique;
import Vitro.Windows.StringUtils;

namespace vt::windows
{
	export class SharedLibrary : public SharedLibraryBase
	{
	public:
		[[nodiscard]] bool reload() final override
		{
			libraryHandle.reset(makeLibraryHandle());
			return libraryHandle != nullptr;
		}

		void* getHandle() final override
		{
			return libraryHandle;
		}

	protected:
		SharedLibrary(std::string_view const name) : name(widenString(name)), libraryHandle(makeLibraryHandle())
		{}

		void* loadSymbolAddress(std::string_view const symbol) const final override
		{
			return ::GetProcAddress(libraryHandle, symbol.data());
		}

	private:
		std::wstring name;
		Unique<HMODULE, ::FreeLibrary> libraryHandle;

		HMODULE makeLibraryHandle() const
		{
			return ::LoadLibrary(name.data());
		}
	};
}
