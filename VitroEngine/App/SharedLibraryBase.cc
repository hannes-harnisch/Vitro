module;
#include <string_view>
export module Vitro.App.SharedLibraryBase;

export class SharedLibraryBase
{
public:
	[[nodiscard]] virtual bool reload() = 0;

	template<typename TSymbol> TSymbol* loadSymbol(std::string_view symbol) const
	{
		return static_cast<TSymbol*>(loadSymbolAddress(symbol));
	}

protected:
	virtual void* loadSymbolAddress(std::string_view symbol) const = 0;
};
