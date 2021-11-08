module;
#include <format>
#include <string>
export module vt.Core.Rect;

namespace vt
{
	export struct Viewport
	{
		static constexpr float MAX_DEPTH = 1.0f;

		float x			= 0;
		float y			= 0;
		float width		= 0;
		float height	= 0;
		float min_depth = 0;
		float max_depth = MAX_DEPTH;

		std::string to_string() const
		{
			return std::format("[{}, {} | {}, {} | {}, {}]", x, y, width, height, min_depth, max_depth);
		}
	};

	// Specifies dimensions in 2D.
	export struct Extent
	{
		unsigned width	= 0;
		unsigned height = 0;

		unsigned area() const
		{
			return width * height;
		}

		bool zero() const
		{
			return width == 0 || height == 0;
		}

		std::string to_string() const
		{
			return std::format("[{}, {}]", width, height);
		}
	};

	// Describes an area offset by the given coordinates.
	export struct Rectangle
	{
		int		 x		= 0;
		int		 y		= 0;
		unsigned width	= 0;
		unsigned height = 0;

		unsigned area() const
		{
			return width * height;
		}

		bool zero() const
		{
			return width == 0 || height == 0;
		}

		Extent extent() const
		{
			return {width, height};
		}

		std::string to_string() const
		{
			return std::format("[{}, {} | {}, {}]", x, y, width, height);
		}
	};

	// Specifies dimensions in 3D.
	export struct Expanse
	{
		unsigned width	= 0;
		unsigned height = 0;
		unsigned depth	= 0;

		unsigned volume() const
		{
			return width * height * depth;
		}

		// True if the volume is zero.
		bool zero() const
		{
			return width == 0 || height == 0 || depth == 0;
		}

		std::string to_string() const
		{
			return std::format("[{}, {}, {}]", width, height, depth);
		}
	};
}
