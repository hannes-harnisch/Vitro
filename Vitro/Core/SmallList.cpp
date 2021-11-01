module;
#include <vector>
export module vt.Core.SmallList;

namespace vt
{
	// This will be replaced with a dynamic array container optimized for small sizes, eventually.
	export template<typename T> using SmallList = std::vector<T>;
}
