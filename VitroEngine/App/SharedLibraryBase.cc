module;
#include <string_view>
export module Vitro.App.SharedLibraryBase;

namespace vt
{
	export class SharedLibraryBase
	{
	public:
		[[nodiscard]] virtual bool reload() = 0;
		virtual void* handle()				= 0;

	protected:
		virtual void* loadSymbolAddress(std::string_view symbol) const = 0;
	};
}
