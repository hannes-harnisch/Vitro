module;
#include "Core/Macros.hpp"

#if VT_DEBUG
	#define DEBUG 1
#endif
#define HH_ASSERT(CONDITION, MESSAGE) VT_ASSERT_PURE(CONDITION, MESSAGE)
#include <FixedList/Include/FixedList.hpp>
export module vt.Core.FixedList;

namespace vt
{
	export template<typename T, size_t Capacity> using FixedList = hh::FixedList<T, Capacity>;
}
