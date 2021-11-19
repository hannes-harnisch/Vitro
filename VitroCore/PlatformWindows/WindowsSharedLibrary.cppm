module;
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
		WindowsSharedLibrary(std::string_view path);

		void*	load_symbol(char const name[]) const;
		void	unload();
		void	try_reload();
		HMODULE native_handle();

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
