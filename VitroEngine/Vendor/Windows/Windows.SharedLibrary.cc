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
			library = make_library();
			return library.get() != nullptr;
		}

		void* handle() final override
		{
			return library.get();
		}

	protected:
		SharedLibrary(std::string_view path) : path(widen_string(path)), library(make_library())
		{}

		void* load_symbol_address(std::string_view symbol) const final override
		{
			return ::GetProcAddress(library.get(), symbol.data());
		}

	private:
		std::wstring				   path;
		Unique<HMODULE, ::FreeLibrary> library;

		decltype(library) make_library() const
		{
			auto raw_library = ::LoadLibraryW(path.data());

			decltype(library) fresh_library(raw_library);
			VT_ENSURE(raw_library, "Failed to find shared library '{}'.", narrow_string(path));

			return fresh_library;
		}
	};
}
