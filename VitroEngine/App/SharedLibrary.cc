module;
#include <stdexcept>
#include <string_view>
export module Vitro.App.SharedLibrary;

import Vitro.VT_SYSTEM.SharedLibrary;

export class SharedLibrary : public VT_SYSTEM::SharedLibrary
{
public:
	SharedLibrary(std::string_view const name) : VT_SYSTEM::SharedLibrary(name)
	{
		if(!handle())
			throw std::runtime_error("Shared library file not found.");
	}

	template<typename TSymbol> TSymbol* loadSymbol(std::string_view const symbol) const
	{
		return static_cast<TSymbol*>(loadSymbolAddress(symbol));
	}

private:
};
