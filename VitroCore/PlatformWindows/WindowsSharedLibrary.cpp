module;
#include "VitroCore/Macros.hpp"
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
			auto symbol = ::GetProcAddress(library.get(), name);
			check_winapi_error(symbol, "Failed to load symbol from shared library.");
			return symbol;
		}

		void unload()
		{
			library.reset();
		}

		void try_reload()
		{
			auto module = ::LoadLibrary(path.data());
			check_winapi_error(module, "Failed to find shared library.");
			library.reset(module);
		}

		HMODULE native_handle()
		{
			return library.get();
		}

	private:
		struct LibraryDeleter
		{
			using pointer = HMODULE;
			void operator()(HMODULE module) const
			{
				auto succeeded = ::FreeLibrary(module);
				check_winapi_error(succeeded, "Failed to free shared library.");
			}
		};

		std::wstring							 path;
		std::unique_ptr<HMODULE, LibraryDeleter> library;
	};
}
