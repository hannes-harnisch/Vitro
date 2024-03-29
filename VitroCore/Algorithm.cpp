module;
#include <array>
#include <concepts>
#include <string_view>
#include <vector>
export module vt.Core.Algorithm;

import vt.Core.ContainerTraits;

namespace vt
{
	export template<typename Container>
	constexpr bool contains(Container const& container, typename ContainerTraits<Container>::const_reference value)
	{
		return std::find(std::begin(container), std::end(container), value) != std::end(container);
	}

	export void remove_first_of(std::string& str, std::string_view pattern)
	{
		size_t i = str.find(pattern);
		if(i != std::string::npos)
			str.erase(i, pattern.length());
	}

	export void remove_all_of(std::string& str, std::string_view pattern)
	{
		for(size_t i = str.find(pattern); i != std::string::npos; i = str.find(pattern))
			str.erase(i, pattern.length());
	}

	// Split a string by a delimiter string or character.
	export std::vector<std::string_view> split(std::string_view str, auto delimiter)
	{
		std::vector<std::string_view> tokens;

		size_t offset = 0, occurrence = 0;
		while(occurrence != std::string_view::npos)
		{
			occurrence = str.find(delimiter, offset);
			tokens.emplace_back(str.substr(offset, occurrence - offset));

			size_t delimiter_length;
			if constexpr(std::same_as<decltype(delimiter), char>)
				delimiter_length = 1;
			else
				delimiter_length = delimiter.size();
			offset = occurrence + delimiter_length;
		}

		return tokens;
	}

	// Split a string by a delimiter, but only up to N substrings.
	export template<size_t N> std::array<std::string_view, N> split(std::string_view str, auto delimiter)
	{
		std::array<std::string_view, N> tokens;

		size_t offset = 0, occurrence = 0, count = 0;
		while(count != N && occurrence != std::string_view::npos)
		{
			occurrence		= str.find(delimiter, offset);
			tokens[count++] = str.substr(offset, occurrence - offset);

			size_t delimiter_length;
			if constexpr(std::same_as<decltype(delimiter), char>)
				delimiter_length = 1;
			else
				delimiter_length = delimiter.size();
			offset = occurrence + delimiter_length;
		}

		return tokens;
	}
}
