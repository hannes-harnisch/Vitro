module;
#include <optional>
#include <string_view>
export module Vitro.App.SharedLibrary;

import Vitro.VE_SYSTEM.SharedLibrary;

export class SharedLibrary : public VE_SYSTEM::SharedLibrary
{
public:
	static std::optional<SharedLibrary> from(std::string_view name)
	{
		auto handle = createLibraryHandle(name);
		if(handle)
			return SharedLibrary(handle, name);
		else
			return {};
	}

private:
	using VE_SYSTEM::SharedLibrary::SharedLibrary;
};
