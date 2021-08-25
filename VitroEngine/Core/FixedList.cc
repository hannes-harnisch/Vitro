﻿module;
#include "Core/Macros.hh"

#if VT_DEBUG
	#define DEBUG 1
#endif
#define HH_ASSERT(condition, message) VT_ASSERT_PURE(condition, message)
#include <FixedList/include/FixedList.hpp>
export module vt.Core.FixedList;

namespace vt
{
	export template<typename T, size_t Capacity> using FixedList = hh::FixedList<T, Capacity>;
}
