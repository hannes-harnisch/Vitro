module;
#include <format>
#include <string>
export module Vitro.Math.Rectangle;

namespace vt
{
	export struct Rectangle
	{
		int x			= 0;
		int y			= 0;
		unsigned width	= 0;
		unsigned height = 0;

		std::string toString() const
		{
			return std::format("[{}, {} | {}, {}]", x, y, width, height);
		}
	};

	export struct Extent
	{
		unsigned width	= 0;
		unsigned height = 0;

		std::string toString() const
		{
			return std::format("[{}, {}]", width, height);
		}
	};

	export struct Viewport
	{
		constexpr static float UpperDepthLimit = 1.0f;

		float x		   = 0;
		float y		   = 0;
		float width	   = 0;
		float height   = 0;
		float minDepth = 0;
		float maxDepth = UpperDepthLimit;

		std::string toString() const
		{
			return std::format("[{}, {} | {}, {} | {}, {}]", x, y, width, height, minDepth, maxDepth);
		}
	};
}
