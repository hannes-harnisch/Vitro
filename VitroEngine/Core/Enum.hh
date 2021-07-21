#pragma once

// TODO: make this a module after stability fixes in MSVC

#include <algorithm>
#include <climits>
#include <string_view>

#define MAGIC_ENUM_RANGE_MIN (INT16_MIN + 1)
#define MAGIC_ENUM_RANGE_MAX (INT16_MAX - 1)
#include <magic_enum/include/magic_enum.hpp>

namespace vt
{
	template<typename TEnum> constexpr std::optional<TEnum> toEnum(std::string_view name)
	{
		return magic_enum::enum_cast<TEnum>(name);
	}

	template<typename TEnum> constexpr size_t getEnumCount()
	{
		return magic_enum::enum_count<TEnum>();
	}

	constexpr std::optional<size_t> getEnumIndex(auto enumValue)
	{
		return magic_enum::enum_index(enumValue);
	}

	constexpr std::string_view getEnumName(auto enumValue)
	{
		return magic_enum::enum_name(enumValue);
	}

	template<typename TEnum> constexpr size_t sizeFromEnumMax()
	{
		auto values = magic_enum::enum_values<TEnum>();
		auto max	= std::max_element(values.begin(), values.end());
		return static_cast<size_t>(*max) + 1;
	}
}
