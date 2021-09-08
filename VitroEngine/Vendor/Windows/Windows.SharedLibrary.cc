module;
#include "Core/Macros.hh"
#include "Windows.API.hh"

#include <string_view>
export module vt.Windows.SharedLibrary;

import vt.Core.Unique;
import vt.Windows.Utils;

namespace vt::windows
{
	export class SharedLibrary
	{
	protected:
		SharedLibrary(std::string_view path) : path(widen_string(path))
		{
			initialize_library();
		}

		void* load_symbol(char const name[]) const
		{
			return ::GetProcAddress(library.get(), name);
		}

		[[nodiscard]] bool reload()
		{
			initialize_library();
			return library.get() != nullptr;
		}

		void* get_handle()
		{
			return library.get();
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
