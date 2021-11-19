module;
#include <array>
#include <concepts>
#include <string_view>
#include <vector>
module vt.Core.Algorithm;

import vt.Core.ContainerTraits;

namespace vt
{
	void remove_first_of(std::string& str, std::string_view pattern)
	{
		size_t i = str.find(pattern);
		if(i != std::string::npos)
			str.erase(i, pattern.length());
	}

	void remove_all_of(std::string& str, std::string_view pattern)
	{
		for(size_t i = str.find(pattern); i != std::string::npos; i = str.find(pattern))
			str.erase(i, pattern.length());
	}

	std::vector<std::string_view> split(std::string_view str, char delimiter)
	{
		std::vector<std::string_view> tokens;

		size_t offset = 0, occurrence = 0;
		while(occurrence != std::string_view::npos)
		{
			occurrence = str.find(delimiter, offset);
			tokens.emplace_back(str.substr(offset, occurrence - offset));
			offset = occurrence + 1;
		}

		return tokens;
	}

	std::vector<std::string_view> split(std::string_view str, std::string_view delimiter)
	{
		std::vector<std::string_view> tokens;

		size_t offset = 0, occurrence = 0;
		while(occurrence != std::string_view::npos)
		{
			occurrence = str.find(delimiter, offset);
			tokens.emplace_back(str.substr(offset, occurrence - offset));
			offset = occurrence + delimiter.size();
		}

		return tokens;
	}
}
