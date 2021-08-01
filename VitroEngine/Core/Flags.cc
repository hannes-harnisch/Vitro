module;
#include <type_traits>
#include <utility>
export module Vitro.Core.Flags;

namespace vt
{
	template<typename T> constexpr std::underlying_type_t<T> toUnderlying(T value)
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}

	export template<typename T> constexpr T operator&(T left, T right)
	{
		return static_cast<T>(toUnderlying(left) & toUnderlying(right));
	}

	export template<typename T> constexpr T operator|(T left, T right)
	{
		return static_cast<T>(toUnderlying(left) | toUnderlying(right));
	}

	export template<typename T> constexpr T operator^(T left, T right)
	{
		return static_cast<T>(toUnderlying(left) ^ toUnderlying(right));
	}

	export template<typename T> constexpr T operator~(T value)
	{
		return static_cast<T>(~toUnderlying(value));
	}

	export template<typename T> constexpr T& operator&=(T& left, T right)
	{
		return left = left & right;
	}

	export template<typename T> constexpr T& operator|=(T& left, T right)
	{
		return left = left | right;
	}

	export template<typename T> constexpr T& operator^=(T& left, T right)
	{
		return left = left ^ right;
	}

	export constexpr auto bit(auto position)
	{
		return 1 << position;
	}
}
