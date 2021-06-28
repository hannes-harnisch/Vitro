#pragma once
// TODO ICE
#include <algorithm>
#include <climits>

#define MAGIC_ENUM_RANGE_MIN (INT16_MIN + 1)
#define MAGIC_ENUM_RANGE_MAX (INT16_MAX - 1)
#include <magic_enum/magic_enum.hpp>

using magic_enum::enum_cast;
using magic_enum::enum_count;
using magic_enum::enum_index;
using magic_enum::enum_name;
using magic_enum::enum_values;

namespace vt
{
	template<typename E> constexpr size_t sizeFromEnumMax()
	{
		auto const values = enum_values<E>();
		auto const max	  = std::max_element(values.begin(), values.end());
		return static_cast<size_t>(*max) + 1;
	}
}
