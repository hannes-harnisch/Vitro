﻿module;
#include "Core/Macros.hh"

#include <algorithm>
#include <string_view>
#include <typeinfo>

#if !VT_COMPILER_MSVC
	#include <cxxabi.h>
#endif
export module vt.Core.Reflect;

import vt.Core.Algorithm;

namespace vt
{
	constexpr std::string demangle(std::string_view symbol)
	{
#if VT_COMPILER_MSVC
		std::string name(symbol);
		remove_first_of(name, "class vt::");
		remove_first_of(name, "struct vt::");
		remove_first_of(name, "union vt::");
		remove_first_of(name, "enum vt::");
#else
		int			status;
		char const* name_buffer = abi::__cxa_demangle(symbol.data(), nullptr, nullptr, &status);
		std::string name(name_buffer);
		std::free(name_buffer);
#endif
		return name;
	}

	export std::string name_of(auto&& object)
	{
		return demangle(typeid(object).name());
	}
}
