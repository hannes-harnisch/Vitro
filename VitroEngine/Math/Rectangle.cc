module;
#include <cstdint>
#include <format>
#include <string>
export module Vitro.Math.Rectangle;

namespace vt
{
	export struct Rectangle
	{
		uint32_t width = 0, height = 0;

		std::string toString() const
		{
			return std::format("[{}, {}]", width, height);
		}
	};
}
