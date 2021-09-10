module;
#include "Core/Macros.hh"

#include <string_view>
export module vt.App.SharedLibrary;

import vt.VT_SYSTEM_MODULE.SharedLibrary;

namespace vt
{
	using SystemSharedLibrary = VT_SYSTEM_NAME::VT_PASTE(VT_SYSTEM_MODULE, SharedLibrary);

	export class SharedLibrary : private SystemSharedLibrary
	{
	public:
		SharedLibrary(std::string_view path) : SystemSharedLibrary(path)
		{}

		template<typename TSymbol> TSymbol* load_symbol(char const name[]) const
		{
			return static_cast<TSymbol*>(SystemSharedLibrary::load_symbol(name));
		}

		void reload()
		{
			unload();
			try_reload();
		}

		auto native_handle()
		{
			return SystemSharedLibrary::native_handle();
		}
	};
}
