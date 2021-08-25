module;
#include <format>
#include <string>
export module vt.Core.Rectangle;

namespace vt
{
	export struct Rectangle
	{
		int		 x		= 0;
		int		 y		= 0;
		unsigned width	= 0;
		unsigned height = 0;

		std::string to_string() const
		{
			return std::format("[{}, {} | {}, {}]", x, y, width, height);
		}
	};

	export struct Extent
	{
		unsigned width	= 0;
		unsigned height = 0;

		std::string to_string() const
		{
			return std::format("[{}, {}]", width, height);
		}
	};

	export struct Viewport
	{
		static constexpr float UpperDepthLimit = 1.0f;

		float x			= 0;
		float y			= 0;
		float width		= 0;
		float height	= 0;
		float min_depth = 0;
		float max_depth = UpperDepthLimit;

		std::string to_string() const
		{
			return std::format("[{}, {} | {}, {} | {}, {}]", x, y, width, height, min_depth, max_depth);
		}
	};
}
