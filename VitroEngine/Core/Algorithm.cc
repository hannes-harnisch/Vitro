module;
#include <string>
#include <string_view>
#include <vector>
export module Vitro.Core.Algorithm;

namespace vt
{
	// Helper for API calls that expect array sizes as unsigned int instead of size_t.
	export constexpr unsigned count(auto const& container) noexcept
	{
		return static_cast<unsigned>(std::size(container));
	}

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

	export constexpr std::vector<std::string_view> split(std::string_view str, std::string_view delimiter)
	{
		std::vector<std::string_view> tokens;

		size_t offset = 0, occurrence = 0;
		while(occurrence != std::string_view::npos)
		{
			occurrence = str.find(delimiter, offset);
			tokens.emplace_back(str.substr(offset, occurrence - offset));
			offset = occurrence + delimiter.length();
		}

		return tokens;
	}
}
