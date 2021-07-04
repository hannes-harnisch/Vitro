module;
#include "Trace/Assert.hh"

#include <string_view>
export module Vitro.App.SharedLibrary;

import Vitro.VT_SYSTEM_MODULE.SharedLibrary;

namespace vt
{
	export class SharedLibrary : public VT_SYSTEM_NAME::SharedLibrary
	{
	public:
		SharedLibrary(std::string_view const name) : VT_SYSTEM_NAME::SharedLibrary(name)
		{
			vtEnsure(handle(), "Shared library file not found.");
		}

		template<typename TSymbol> TSymbol* loadSymbol(std::string_view const symbol) const
		{
			return static_cast<TSymbol*>(loadSymbolAddress(symbol));
		}
	};
}
