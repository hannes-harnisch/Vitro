module;
#include <unordered_map>
export module Vitro.Core.HashMap;

namespace vt
{
	export template<typename TKey, typename TValue> using HashMap = std::unordered_map<TKey, TValue>;
}
