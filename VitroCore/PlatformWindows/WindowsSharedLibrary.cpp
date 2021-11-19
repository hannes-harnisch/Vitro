module;
#include "WindowsAPI.hpp"

#include <memory>
#include <string_view>
module vt.Core.Windows.SharedLibrary;

import vt.Core.Windows.Utils;

namespace vt::windows
{
	WindowsSharedLibrary::WindowsSharedLibrary(std::string_view path) : path(widen_string(path))
	{
		try_reload();
	}

	void* WindowsSharedLibrary::load_symbol(char const name[]) const
	{
		auto symbol = ::GetProcAddress(library.get(), name);
		check_winapi_error(symbol, "Failed to load symbol from shared library.");
		return symbol;
	}

	void WindowsSharedLibrary::unload()
	{
		library.reset();
	}

	void WindowsSharedLibrary::try_reload()
	{
		auto module = ::LoadLibrary(path.data());
		check_winapi_error(module, "Failed to find shared library.");
		library.reset(module);
	}

	HMODULE WindowsSharedLibrary::native_handle()
	{
		return library.get();
	}
}
