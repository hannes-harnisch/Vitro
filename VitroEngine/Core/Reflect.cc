module;
#include "Core/Intrinsics.hh"

#include <algorithm>
#include <string_view>
#include <typeinfo>

#if !VE_COMPILER_MSVC
	#include <cxxabi.h>
#endif
export module Vitro.Core.Reflect;

import Vitro.Core.StringUtils;

constexpr std::string demangle(std::string_view symbol)
{
#if VE_COMPILER_MSVC
	std::string name(symbol);
	removeFirstOf(name, "class ");
	removeFirstOf(name, "struct ");
	removeFirstOf(name, "union ");
	removeFirstOf(name, "enum ");
#else
	int status;
	char* nameBuffer = abi::__cxa_demangle(symbol.data(), nullptr, nullptr, &status);
	std::string name(nameBuffer);
	std::free(nameBuffer);
#endif
	return name;
}

export std::string nameOf(auto&& object)
{
	return demangle(typeid(object).name());
}
