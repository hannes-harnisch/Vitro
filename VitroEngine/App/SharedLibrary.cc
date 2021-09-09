module;
#include "Core/Macros.hh"

#include <string_view>
export module vt.App.SharedLibrary;

import vt.VT_SYSTEM_MODULE.SharedLibrary;

namespace vt
{
	using SharedLibraryBase = VT_SYSTEM_NAME::VT_PASTE(VT_SYSTEM_MODULE, SharedLibrary);

	export class SharedLibrary : public SharedLibraryBase
	{
	public:
		SharedLibrary(std::string_view path) : SharedLibraryBase(path)
		{}

		template<typename TSymbol> TSymbol* load_symbol(char const name[]) const
		{
			return static_cast<TSymbol*>(SharedLibraryBase::load_symbol(name));
		}

		void reload()
		{
			unload();
			try_reload();
		}

		auto native_handle()
		{
			return SharedLibraryBase::native_handle();
		}
	};
}
