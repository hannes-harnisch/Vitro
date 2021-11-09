module;
#include <cmath>
#include <cstdint>
export module vt.Graphics.AssetResourceSpecification;

import vt.Core.Enum;
import vt.Core.Rect;
import vt.Core.Specification;

namespace vt
{
	// Bit flags specifying what a buffer will be used for.
	export enum class BufferUsage : uint16_t {
		CopySrc	  = bit(0),
		CopyDst	  = bit(1),
		Untyped	  = bit(2), // Corresponds to the descriptor type "Buffer".
		RwUntyped = bit(3), // Corresponds to the descriptor type "RwBuffer".
		Uniform	  = bit(4),
		Storage	  = bit(5),
		Index	  = bit(6),
		Vertex	  = bit(7),
		Indirect  = bit(8),
		Upload	  = bit(9),	 // Buffer will be used for uploading data to the GPU.
		Readback  = bit(10), // Buffer will be used for reading back data from the GPU.
	};
	export template<> constexpr inline bool ENABLE_BIT_OPERATORS_FOR<BufferUsage> = true;

	export struct BufferSpecification
	{
		Positive<size_t>	  size;
		Positive<unsigned>	  stride;
		Explicit<BufferUsage> usage;
	};

	// Order, size and type of pixel components in an image. To be read in terms of components, so Rgba32Float has a pixel
	// layout of 32-bits per color component, with type float.
	export enum class ImageFormat : uint8_t {
		Unknown,
		Rgba32Float,
		Rgba32UInt,
		Rgba32SInt,
		Rgb32Float,
		Rgb32UInt,
		Rgb32SInt,
		Rgba16Float,
		Rgba16UNorm,
		Rgba16UInt,
		Rgba16SNorm,
		Rgba16SInt,
		Rg32Float,
		Rg32UInt,
		Rg32SInt,
		R32G8X24Typeless,
		D32FloatS8X24UInt,
		Rgb10A2UNorm,
		Rgb10A2UInt,
		Rg11B10Float,
		Rgba8UNorm,
		Rgba8UNormSrgb,
		Rgba8UInt,
		Rgba8SNorm,
		Rgba8SInt,
		Bgra8UNorm,
		Bgra8UNormSrgb,
		Rg16Float,
		Rg16UNorm,
		Rg16UInt,
		Rg16SNorm,
		Rg16SInt,
		R32Typeless,
		D32Float,
		R32Float,
		R32UInt,
		R32SInt,
		R24G8Typeless,
		D24UNormS8UInt,
		Rg8UNorm,
		Rg8UInt,
		Rg8SNorm,
		Rg8SInt,
		R16Typeless,
		R16Float,
		D16UNorm,
		R16UNorm,
		R16UInt,
		R16SNorm,
		R16SInt,
		R8UNorm,
		R8UInt,
		R8SNorm,
		R8SInt,
		Bc1UNorm,	  // Uses block compression.
		Bc1UNormSrgb, // Uses block compression.
		Bc2UNorm,	  // Uses block compression.
		Bc2UNormSrgb, // Uses block compression.
		Bc3UNorm,	  // Uses block compression.
		Bc3UNormSrgb, // Uses block compression.
		Bc4UNorm,	  // Uses block compression.
		Bc4SNorm,	  // Uses block compression.
		Bc5UNorm,	  // Uses block compression.
		Bc5SNorm,	  // Uses block compression.
		Bc6HUFloat16, // Uses block compression.
		Bc6HSFloat16, // Uses block compression.
		Bc7UNorm,	  // Uses block compression.
		Bc7UNormSrgb, // Uses block compression.
	};

	// Specifies a purpose for which an image will be used and for which there is a recommended memory layout to which it should
	// be transitioned.
	export enum class ImageLayout : uint8_t {
		Undefined,
		General,
		CopySource,
		CopyDestination,
		ColorAttachment,
		DepthStencilAttachment,
		DepthStencilReadOnly,
		ShaderResource,
		FragmentShaderResource,
		UnorderedAccess,
		Presentable,
	};

	export enum class ImageDimension : uint8_t {
		Image1D,
		Image2D,
		Image3D,
		Cube,
		Array1D,
		Array2D,
		CubeArray,
	};

	export enum class ImageUsage : uint8_t {
		CopySrc			= bit(0),
		CopyDst			= bit(1),
		Sampled			= bit(2),
		Storage			= bit(3),
		ColorAttachment = bit(4),
		DepthStencil	= bit(5),
		Transient		= bit(6),
		InputAttachment = bit(7),
	};
	export template<> constexpr inline bool ENABLE_BIT_OPERATORS_FOR<ImageUsage> = true;

	// Describes the properties of an image to be created. If the dimension is 2D, expanse depth will be interpreted as the
	// count of an image array.
	export struct ImageSpecification
	{
		Explicit<Expanse>		 expanse;
		Explicit<ImageDimension> dimension;
		Explicit<ImageFormat>	 format;
		Positive<uint8_t>		 mip_count	  = calc_default_mip_count();
		uint8_t					 sample_count = 1;
		Explicit<ImageUsage>	 usage;

		uint8_t calc_default_mip_count() const
		{
			unsigned max   = std::max(expanse.width, expanse.height);
			double	 count = std::floor(std::log2(max)) + 1;
			return static_cast<uint8_t>(count);
		}
	};
}
