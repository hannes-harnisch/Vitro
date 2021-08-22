module;
#include <string_view>
export module Vitro.App.SharedLibrary;

import Vitro.VT_SYSTEM_MODULE.SharedLibrary;

namespace vt
{
	export class SharedLibrary : public VT_SYSTEM_NAME::SharedLibrary
	{
	public:
		SharedLibrary(std::string_view path) : VT_SYSTEM_NAME::SharedLibrary(path)
		{}

		template<typename TSymbol> TSymbol* loadSymbol(std::string_view symbol) const
		{
			return static_cast<TSymbol*>(loadSymbolAddress(symbol));
		}
	};
}
