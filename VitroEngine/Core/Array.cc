module;
#include "Core/Macros.hh"

#if VT_DEBUG
	#define DEBUG 1
#endif
#define HH_ASSERT(condition, message) VT_ASSERT_PURE(condition, message)
#include <Array/Include/Array.hpp>
export module vt.Core.Array;

namespace vt
{
	export template<typename T> using Array = hh::Array<T>;
}
