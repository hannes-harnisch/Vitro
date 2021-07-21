module;
#include <string>
export module Vitro.Core.StringUtils;

namespace vt
{
	export constexpr void removeFirstOf(std::string& str, std::string_view pattern)
	{
		size_t i = str.find(pattern);
		if(i != std::string::npos)
			str.erase(i, pattern.length());
	}

	export constexpr void removeAllOf(std::string& str, std::string_view pattern)
	{
		for(size_t i = str.find(pattern); i != std::string::npos; i = str.find(pattern))
			str.erase(i, pattern.length());
	}
}
