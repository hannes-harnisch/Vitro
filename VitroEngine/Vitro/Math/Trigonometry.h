#pragma once

namespace Vitro
{
	template<typename F> constexpr F Pi = static_cast<F>(3.14159265358979L);

	template<typename F = float, typename N> constexpr auto Degrees(N radians)
	{
		return static_cast<F>(180) / Pi<F> * radians;
	}

	template<typename F = float, typename N> constexpr auto Radians(N degrees)
	{
		return degrees / (static_cast<F>(180) / Pi<F>);
	}
}
