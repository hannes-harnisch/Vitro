module;
#include "Trace/Assert.hh"
#include <Array/Include/Array.hpp>
export module Vitro.Core.Array;

namespace vt
{
	export template<typename T> using Array = hh::Array<T>;
}
