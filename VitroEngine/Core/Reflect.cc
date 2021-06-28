module;
#include "Core/Intrinsics.hh"

#include <algorithm>
#include <string_view>
#include <typeinfo>

#if !VT_COMPILER_MSVC
	#include <cxxabi.h>
#endif
export module Vitro.Core.Reflect;

import Vitro.Core.StringUtils;

namespace vt
{
	constexpr std::string demangle(std::string_view const symbol)
	{
#if VT_COMPILER_MSVC
		std::string name(symbol);
		removeFirstOf(name, "class vt::");
		removeFirstOf(name, "struct vt::");
		removeFirstOf(name, "union vt::");
		removeFirstOf(name, "enum vt::");
#else
		int status;
		char const* const nameBuffer = abi::__cxa_demangle(symbol.data(), nullptr, nullptr, &status);
		std::string name(nameBuffer);
		std::free(nameBuffer);
#endif
		return name;
	}

	export std::string nameOf(auto&& object)
	{
		return demangle(typeid(object).name());
	}
}
