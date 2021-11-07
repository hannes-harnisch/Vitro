module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Image;

import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.D3D12.Resource;

namespace vt::d3d12
{
	export DXGI_FORMAT convert_image_format(ImageFormat format)
	{
		using enum ImageFormat;
		switch(format)
		{ // clang-format off
			case Unknown:			return DXGI_FORMAT_UNKNOWN;
			case Rgba32Float:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case Rgba32UInt:		return DXGI_FORMAT_R32G32B32A32_UINT;
			case Rgba32SInt:		return DXGI_FORMAT_R32G32B32A32_SINT;
			case Rgb32Float:		return DXGI_FORMAT_R32G32B32_FLOAT;
			case Rgb32UInt:			return DXGI_FORMAT_R32G32B32_UINT;
			case Rgb32SInt:			return DXGI_FORMAT_R32G32B32_SINT;
			case Rgba16Float:		return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case Rgba16UNorm:		return DXGI_FORMAT_R16G16B16A16_UNORM;
			case Rgba16UInt:		return DXGI_FORMAT_R16G16B16A16_UINT;
			case Rgba16SNorm:		return DXGI_FORMAT_R16G16B16A16_SNORM;
			case Rgba16SInt:		return DXGI_FORMAT_R16G16B16A16_SINT;
			case Rg32Float:			return DXGI_FORMAT_R32G32_FLOAT;
			case Rg32UInt:			return DXGI_FORMAT_R32G32_UINT;
			case Rg32SInt:			return DXGI_FORMAT_R32G32_SINT;
			case R32G8X24Typeless:	return DXGI_FORMAT_R32G8X24_TYPELESS;
			case D32FloatS8X24UInt:	return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			case Rgb10A2UNorm:		return DXGI_FORMAT_R10G10B10A2_UNORM;
			case Rgb10A2UInt:		return DXGI_FORMAT_R10G10B10A2_UINT;
			case Rg11B10Float:		return DXGI_FORMAT_R11G11B10_FLOAT;
			case Rgba8UNorm:		return DXGI_FORMAT_R8G8B8A8_UNORM;
			case Rgba8UNormSrgb:	return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case Rgba8UInt:			return DXGI_FORMAT_R8G8B8A8_UINT;
			case Rgba8SNorm:		return DXGI_FORMAT_R8G8B8A8_SNORM;
			case Rgba8SInt:			return DXGI_FORMAT_R8G8B8A8_SINT;
			case Bgra8UNorm:		return DXGI_FORMAT_B8G8R8A8_UNORM;
			case Bgra8UNormSrgb:	return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			case Rg16Float:			return DXGI_FORMAT_R16G16_FLOAT;
			case Rg16UNorm:			return DXGI_FORMAT_R16G16_UNORM;
			case Rg16UInt:			return DXGI_FORMAT_R16G16_UINT;
			case Rg16SNorm:			return DXGI_FORMAT_R16G16_SNORM;
			case Rg16SInt:			return DXGI_FORMAT_R16G16_SINT;
			case R32Typeless:		return DXGI_FORMAT_R32_TYPELESS;
			case D32Float:			return DXGI_FORMAT_D32_FLOAT;
			case R32Float:			return DXGI_FORMAT_R32_FLOAT;
			case R32UInt:			return DXGI_FORMAT_R32_UINT;
			case R32SInt:			return DXGI_FORMAT_R32_SINT;
			case R24G8Typeless:		return DXGI_FORMAT_R24G8_TYPELESS;
			case D24UNormS8UInt:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case Rg8UNorm:			return DXGI_FORMAT_R8G8_UNORM;
			case Rg8UInt:			return DXGI_FORMAT_R8G8_UINT;
			case Rg8SNorm:			return DXGI_FORMAT_R8G8_SNORM;
			case Rg8SInt:			return DXGI_FORMAT_R8G8_SINT;
			case R16Typeless:		return DXGI_FORMAT_R16_TYPELESS;
			case R16Float:			return DXGI_FORMAT_R16_FLOAT;
			case D16UNorm:			return DXGI_FORMAT_D16_UNORM;
			case R16UNorm:			return DXGI_FORMAT_R16_UNORM;
			case R16UInt:			return DXGI_FORMAT_R16_UINT;
			case R16SNorm:			return DXGI_FORMAT_R16_SNORM;
			case R16SInt:			return DXGI_FORMAT_R16_SINT;
			case R8UNorm:			return DXGI_FORMAT_R8_UNORM;
			case R8UInt:			return DXGI_FORMAT_R8_UINT;
			case R8SNorm:			return DXGI_FORMAT_R8_SNORM;
			case R8SInt:			return DXGI_FORMAT_R8_SINT;
			case Bc1UNorm:			return DXGI_FORMAT_BC1_UNORM;
			case Bc1UNormSrgb:		return DXGI_FORMAT_BC1_UNORM_SRGB;
			case Bc2UNorm:			return DXGI_FORMAT_BC2_UNORM;
			case Bc2UNormSrgb:		return DXGI_FORMAT_BC2_UNORM_SRGB;
			case Bc3UNorm:			return DXGI_FORMAT_BC3_UNORM;
			case Bc3UNormSrgb:		return DXGI_FORMAT_BC3_UNORM_SRGB;
			case Bc4UNorm:			return DXGI_FORMAT_BC4_UNORM;
			case Bc4SNorm:			return DXGI_FORMAT_BC4_SNORM;
			case Bc5UNorm:			return DXGI_FORMAT_BC5_UNORM;
			case Bc5SNorm:			return DXGI_FORMAT_BC5_SNORM;
			case Bc6HUFloat16:		return DXGI_FORMAT_BC6H_UF16;
			case Bc6HSFloat16:		return DXGI_FORMAT_BC6H_SF16;
			case Bc7UNorm:			return DXGI_FORMAT_BC7_UNORM;
			case Bc7UNormSrgb:		return DXGI_FORMAT_BC7_UNORM_SRGB;
		}
		VT_UNREACHABLE();
	}

	export D3D12_RESOURCE_STATES convert_image_layout(ImageLayout layout)
	{
		using enum ImageLayout;
		switch(layout)
		{
			case Undefined:
			case General:
			case Presentable:				return D3D12_RESOURCE_STATE_COMMON;
			case CopySource:				return D3D12_RESOURCE_STATE_COPY_SOURCE;
			case CopyDestination:			return D3D12_RESOURCE_STATE_COPY_DEST;
			case ColorAttachment:			return D3D12_RESOURCE_STATE_RENDER_TARGET;
			case DepthStencilAttachment:	return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			case DepthStencilReadOnly:		return D3D12_RESOURCE_STATE_DEPTH_READ;
			case ShaderResource:			return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
												   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			case FragmentShaderResource:	return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			case NonFragmentShaderResource:	return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			case UnorderedAccess:			return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		} // clang-format on
		VT_UNREACHABLE();
	}

	D3D12_RESOURCE_DIMENSION convert_image_dimension(ImageDimension dimension)
	{
		using enum ImageDimension;
		switch(dimension)
		{
			case Image1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
			case Image2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			case Image3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		}
		VT_UNREACHABLE();
	}

	export class D3D12Image : public Resource
	{
	public:
		D3D12Image(ImageSpecification const& spec, D3D12MA::Allocator& allocator)
		{
			D3D12_HEAP_TYPE		  heap_type;
			D3D12_RESOURCE_STATES initial_state;
			if(spec.usage.get() & BufferUsage::Upload)
			{
				heap_type	  = D3D12_HEAP_TYPE_UPLOAD;
				initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;
			}
			else if(spec.usage.get() & BufferUsage::Readback)
			{
				heap_type	  = D3D12_HEAP_TYPE_READBACK;
				initial_state = D3D12_RESOURCE_STATE_COPY_DEST;
			}
			else
			{
				heap_type	  = D3D12_HEAP_TYPE_DEFAULT;
				initial_state = D3D12_RESOURCE_STATE_COMMON; // More specialized resource states are not needed for buffers
			}												 // because of the promotion rules.

			D3D12MA::ALLOCATION_DESC const allocation_desc {
				.HeapType = heap_type,
			};
			D3D12_RESOURCE_DESC const resource_desc {
				.Dimension		  = convert_image_dimension(spec.image_dimension),
				.Alignment		  = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
				.Width			  = spec.expanse.width,
				.Height			  = spec.expanse.height,
				.DepthOrArraySize = spec.expanse.depth,
				.MipLevels		  = spec.mip_count,
				.Format			  = convert_image_format(spec.image_format),
				.SampleDesc {
					.Count	 = spec.sample_count,
					.Quality = 0,
				},
				.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
				.Flags	= D3D12_RESOURCE_FLAG_NONE,
			};
			auto result = allocator.CreateResource(&allocation_desc, &resource_desc, initial_state, nullptr,
												   std::out_ptr(allocation), VT_COM_OUT(resource));
			VT_CHECK_RESULT(result, "Failed to create D3D12 image.");
		}
	};
}
