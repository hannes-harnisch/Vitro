module;
#include <unordered_map>
export module Vitro.Core.HashMap;

namespace vt
{
	export template<typename Key, typename Value> using HashMap = std::unordered_map<Key, Value>;
}
