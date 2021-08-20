module;
#include <algorithm>
#include <bitset>
#include <climits>
#include <concepts>
#include <string>
#include <type_traits>
#include <utility>

#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using string_view = std::string_view;
#define MAGIC_ENUM_RANGE_MIN			   (INT16_MIN + 1)
#define MAGIC_ENUM_RANGE_MAX			   (INT16_MAX - 1)
#include <magic_enum/include/magic_enum.hpp>
export module Vitro.Core.Enum;

// TODO: remove when this is supported in preview
namespace std
{
	template<typename T> [[nodiscard]] constexpr underlying_type_t<T> to_underlying(T val) noexcept
	{
		return static_cast<underlying_type_t<T>>(val);
	}
}

namespace vt
{
	export using magic_enum::enum_cast;
	export using magic_enum::enum_count;
	export using magic_enum::enum_name;

	template<typename T> constexpr size_t sizeFromEnumMax()
	{
		auto values = magic_enum::enum_values<T>();
		auto max	= std::max_element(values.begin(), values.end());
		return static_cast<size_t>(*max) + 1;
	}

	export template<typename T> class EnumBitArray
	{
	public:
		auto operator[](T index)
		{
			return bitset[static_cast<size_t>(index)];
		}

		bool operator[](T index) const
		{
			return bitset[static_cast<size_t>(index)];
		}

		bool test(T position) const
		{
			return bitset.test(static_cast<size_t>(position));
		}

		void set(T position, bool value = true)
		{
			bitset.set(static_cast<size_t>(position), value);
		}

		std::string toString() const
		{
			return bitset.to_string();
		}

	private:
		std::bitset<sizeFromEnumMax<T>()> bitset;
	};

	export constexpr auto bit(auto position)
	{
		return 1 << position;
	}

	export template<typename T>
	requires std::is_enum_v<T>
	constexpr bool EnableFlagsFor = false;

	template<typename T>
	concept EnablesFlags = EnableFlagsFor<T>;

	export template<EnablesFlags T> constexpr T operator&(T left, T right)
	{
		return static_cast<T>(std::to_underlying(left) & std::to_underlying(right));
	}

	export template<EnablesFlags T> constexpr T operator|(T left, T right)
	{
		return static_cast<T>(std::to_underlying(left) | std::to_underlying(right));
	}

	export template<EnablesFlags T> constexpr T operator^(T left, T right)
	{
		return static_cast<T>(std::to_underlying(left) ^ std::to_underlying(right));
	}

	export template<EnablesFlags T> constexpr T operator~(T value)
	{
		return static_cast<T>(~std::to_underlying(value));
	}

	export template<EnablesFlags T> constexpr T& operator&=(T& left, T right)
	{
		return left = left & right;
	}

	export template<EnablesFlags T> constexpr T& operator|=(T& left, T right)
	{
		return left = left | right;
	}

	export template<EnablesFlags T> constexpr T& operator^=(T& left, T right)
	{
		return left = left ^ right;
	}
}
