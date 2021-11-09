module;
#include "Core/Macros.hpp"
export module vt.Graphics.AssetResource;

import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.VT_GPU_API_MODULE.Buffer;
import vt.Graphics.VT_GPU_API_MODULE.Image;
import vt.Graphics.VT_GPU_API_MODULE.Pipeline;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.Buffer;
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.Image;
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.Pipeline;
#endif

namespace vt
{
	using PlatformBuffer = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Buffer)>;

	// Represents a buffer resource usable in various GPU operations.
	export class Buffer : public PlatformBuffer
	{
	public:
		// This constructor is for internal use only.
		Buffer(PlatformBuffer&& platform_buffer, BufferSpecification const& spec) :
			PlatformBuffer(std::move(platform_buffer)), size(static_cast<unsigned>(spec.size)), stride(spec.stride)
		{}

		unsigned get_size() const
		{
			return size;
		}

		unsigned get_stride() const
		{
			return stride;
		}

	private:
		unsigned size;
		unsigned stride;
	};

	using PlatformImage = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Image)>;

	// Represents an image resource usable in various GPU operations.
	export class Image : public PlatformImage
	{
	public:
		// This constructor is for internal use only.
		Image(PlatformImage&& platform_image, ImageSpecification const& spec) :
			PlatformImage(std::move(platform_image)),
			width(static_cast<uint16_t>(spec.expanse.width)),
			height(static_cast<uint16_t>(spec.expanse.height)),
			depth(static_cast<uint16_t>(spec.expanse.depth)),
			mips(spec.mip_count),
			format(spec.format)
		{}

		uint16_t get_width() const
		{
			return width;
		}

		uint16_t get_height() const
		{
			return height;
		}

		uint16_t get_depth() const
		{
			return depth;
		}

		uint8_t count_mips() const
		{
			return mips;
		}

		ImageFormat get_format() const
		{
			return format;
		}

	private:
		uint16_t	width;
		uint16_t	height;
		uint16_t	depth;
		uint8_t		mips;
		ImageFormat format;
	};

	using PlatformComputePipeline = ResourceVariant<VT_GPU_API_VARIANT_ARGS(ComputePipeline)>;
	using PlatformRenderPipeline  = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderPipeline)>;

	// Represents a collection of GPU state necessary to perform specific compute operations.
	export class ComputePipeline : public PlatformComputePipeline
	{
		using PlatformComputePipeline::PlatformComputePipeline;
	};

	// Represents a collection of GPU state necessary to perform specific rendering operations.
	export class RenderPipeline : public PlatformRenderPipeline
	{
		using PlatformRenderPipeline::PlatformRenderPipeline;
	};
}
