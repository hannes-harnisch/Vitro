module;
#include <stdexcept>
#include <string_view>
export module Vitro.App.SharedLibrary;

import Vitro.VE_SYSTEM.SharedLibrary;

export class SharedLibrary : public VE_SYSTEM::SharedLibrary
{
public:
	SharedLibrary(std::string_view const name) : VE_SYSTEM::SharedLibrary(name)
	{
		if(!handle())
			throw std::runtime_error("File not found.");
	}

	template<typename TSymbol> TSymbol* loadSymbol(std::string_view symbol) const
	{
		return static_cast<TSymbol*>(loadSymbolAddress(symbol));
	}

private:
};
