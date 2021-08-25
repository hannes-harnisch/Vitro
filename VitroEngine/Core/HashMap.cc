module;
#include <unordered_map>
export module vt.Core.HashMap;

namespace vt
{
	export template<typename TKey, typename TValue> using HashMap = std::unordered_map<TKey, TValue>;
}
