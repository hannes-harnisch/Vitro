module;
#include "Core/Macros.hh"

#if VT_DEBUG
	#define DEBUG 1
#endif
#define HH_ASSERT(condition, message) vtAssertPure(condition, message)
#include <Array/Include/Array.hpp>
export module Vitro.Core.Array;

namespace vt
{
	export template<typename T> using Array = hh::Array<T>;
}
