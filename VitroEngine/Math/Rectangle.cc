module;
#include <cstdint>
#include <format>
export module Vitro.Math.Rectangle;

export struct Rectangle
{
	uint32_t width {}, height {};

	std::string toString() const
	{
		return std::format("[{}, {}]", width, height);
	}
};
