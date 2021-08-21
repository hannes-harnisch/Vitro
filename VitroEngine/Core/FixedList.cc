﻿module;
#include "Core/Macros.hh"

#if VT_DEBUG
	#define DEBUG 1
#endif
#define HH_ASSERT(condition, message) vtAssertPure(condition, message)
#include <FixedList/include/FixedList.hpp>
export module Vitro.Core.FixedList;

namespace vt
{
	export template<typename T, unsigned Capacity> using FixedList = hh::FixedList<T, Capacity>;
}
