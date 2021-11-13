module;
#include <functional>
export module vt.Core.Ref;

namespace vt
{
	// Acts as an assignable mutable reference to an object.
	export template<typename T> using Ref = std::reference_wrapper<T>;

	// Acts as an assignable const reference to an object.
	export template<typename T> using CRef = std::reference_wrapper<T const>;
}
