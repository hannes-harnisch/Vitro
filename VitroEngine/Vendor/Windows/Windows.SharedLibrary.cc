module;
#include "Core/Macros.hh"
#include "Windows.API.hh"

#include <string_view>
export module vt.Windows.SharedLibrary;

import vt.App.SharedLibraryBase;
import vt.Core.Unique;
import vt.Windows.Utils;

namespace vt::windows
{
	export class SharedLibrary : public SharedLibraryBase
	{
	public:
		[[nodiscard]] bool reload() final override
		{
			initialize_library();
			return library.get() != nullptr;
		}

		void* get_handle() final override
		{
			return library.get();
		}

	protected:
		SharedLibrary(std::string_view path) : path(widen_string(path))
		{
			initialize_library();
		}

		void* load_symbol_address(std::string_view symbol) const final override
		{
			return ::GetProcAddress(library.get(), symbol.data());
		}

	private:
		std::wstring				   path;
		Unique<HMODULE, ::FreeLibrary> library;

		void initialize_library()
		{
			library.reset(::LoadLibraryW(path.data()));
			VT_ENSURE(library, "Failed to find shared library '{}'.", narrow_string(path));
		}
	};
}
