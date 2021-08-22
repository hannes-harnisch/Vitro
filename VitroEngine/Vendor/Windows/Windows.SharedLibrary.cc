module;
#include "Core/Macros.hh"
#include "Windows.API.hh"

#include <string_view>
export module Vitro.Windows.SharedLibrary;

import Vitro.App.SharedLibraryBase;
import Vitro.Core.Unique;
import Vitro.Windows.Utils;

namespace vt::windows
{
	export class SharedLibrary : public SharedLibraryBase
	{
	public:
		[[nodiscard]] bool reload() final override
		{
			library = makeLibrary();
			return library.get() != nullptr;
		}

		void* handle() final override
		{
			return library.get();
		}

	protected:
		SharedLibrary(std::string_view path) : path(widenString(path)), library(makeLibrary())
		{}

		void* loadSymbolAddress(std::string_view symbol) const final override
		{
			return ::GetProcAddress(library.get(), symbol.data());
		}

	private:
		std::wstring				   path;
		Unique<HMODULE, ::FreeLibrary> library;

		decltype(library) makeLibrary() const
		{
			auto rawLibrary = ::LoadLibraryW(path.data());

			decltype(library) freshLibrary(rawLibrary);
			vtEnsure(rawLibrary, "Failed to find shared library '{}'.", narrowString(path));

			return freshLibrary;
		}
	};
}
