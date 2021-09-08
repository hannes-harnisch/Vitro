module;
#include <string_view>
export module vt.App.SharedLibrary;

import vt.VT_SYSTEM_MODULE.SharedLibrary;

namespace vt
{
	export class SharedLibrary : public VT_SYSTEM_NAME::SharedLibrary
	{
		using Base = VT_SYSTEM_NAME::SharedLibrary;

	public:
		SharedLibrary(std::string_view path) : Base(path)
		{}

		template<typename TSymbol> TSymbol* load_symbol(char const name[]) const
		{
			return static_cast<TSymbol*>(Base::load_symbol(name));
		}

		[[nodiscard]] bool reload()
		{
			return Base::reload();
		}

		void* get_handle()
		{
			return Base::get_handle();
		}
	};
}
