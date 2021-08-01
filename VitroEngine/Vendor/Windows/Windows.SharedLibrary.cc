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
			library.reset(makeLibrary());
			return library.get() != nullptr;
		}

		void* handle() final override
		{
			return library.get();
		}

	protected:
		SharedLibrary(std::string_view name) : name(widenString(name)), library(makeLibrary())
		{}

		void* loadSymbolAddress(std::string_view symbol) const final override
		{
			return ::GetProcAddress(library.get(), symbol.data());
		}

	private:
		std::wstring				   name;
		Unique<HMODULE, ::FreeLibrary> library;

		HMODULE makeLibrary() const
		{
			return ::LoadLibrary(name.data());
		}
	};
}
