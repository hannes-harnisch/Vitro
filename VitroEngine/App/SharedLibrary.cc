module;
#include <string_view>
export module vt.App.SharedLibrary;

import vt.VT_SYSTEM_MODULE.SharedLibrary;

namespace vt
{
	export class SharedLibrary : public VT_SYSTEM_NAME::SharedLibrary
	{
	public:
		SharedLibrary(std::string_view path) : VT_SYSTEM_NAME::SharedLibrary(path)
		{}

		template<typename TSymbol> TSymbol* load_symbol(std::string_view symbol) const
		{
			return static_cast<TSymbol*>(load_symbol_address(symbol));
		}
	};
}
