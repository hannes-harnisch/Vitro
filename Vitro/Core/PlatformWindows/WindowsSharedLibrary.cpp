module;
#include "Core/Macros.hpp"
#include "WindowsAPI.hpp"

#include <memory>
#include <string_view>
export module vt.Core.Windows.SharedLibrary;

import vt.Core.Windows.Utils;

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
			library.reset(::LoadLibrary(path.data()));
			VT_ENSURE(library, "Failed to find shared library '{}'.", narrow_string(path));
		}

		HMODULE native_handle()
		{
			return library.get();
		}

	private:
		std::wstring path;

		struct LibraryDeleter
		{
			using pointer = HMODULE;
			void operator()(HMODULE module) const
			{
				BOOL succeeded = ::FreeLibrary(module);
				VT_ASSERT(succeeded, "Failed to free shared library.");
			}
		};
		std::unique_ptr<HMODULE, LibraryDeleter> library;
	};
}
