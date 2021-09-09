module;
#include "Core/Macros.hh"
#include "Windows.API.hh"

#include <string_view>
export module vt.Windows.SharedLibrary;

import vt.Core.Unique;
import vt.Windows.Utils;

namespace vt::windows
{
	export class WindowsSharedLibrary
	{
	protected:
		WindowsSharedLibrary(std::string_view path) : path(widen_string(path))
		{
			try_reload();
		}

		void* load_symbol(char const name[]) const
		{
			return ::GetProcAddress(library.get(), name);
		}

		void unload()
		{
			library.reset();
		}

		void try_reload()
		{
			library.reset(::LoadLibraryW(path.data()));
			VT_ENSURE(library, "Failed to find shared library '{}'.", narrow_string(path));
		}

		HMODULE native_handle()
		{
			return library.get();
		}

	private:
		std::wstring				   path;
		Unique<HMODULE, ::FreeLibrary> library;
	};
}
