module;
#include "Core/Macros.hh"

#define HH_ASSERT(condition, message) vtAssertPure(condition, message)
#include <Array/Include/Array.hpp>
export module Vitro.Core.Array;

namespace vt
{
	export template<typename T> using Array = hh::Array<T>;
}
