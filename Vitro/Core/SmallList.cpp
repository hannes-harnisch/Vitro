module;
#include <vector>
export module vt.Core.SmallList;

namespace vt
{
	export template<typename T> using SmallList = std::vector<T>;
}
