module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
export module vt.Graphics.D3D12.Image;

import vt.Core.LookupTable;
import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.D3D12.Resource;

namespace vt::d3d12
{
	export constexpr inline auto IMAGE_FORMAT_LOOKUP = [] {
		LookupTable<ImageFormat, DXGI_FORMAT> _;
		using enum ImageFormat;

		_[Unknown]			 = DXGI_FORMAT_UNKNOWN;
		_[Rgba32Float]		 = DXGI_FORMAT_R32G32B32A32_FLOAT;
		_[Rgba32UInt]		 = DXGI_FORMAT_R32G32B32A32_UINT;
		_[Rgba32SInt]		 = DXGI_FORMAT_R32G32B32A32_SINT;
		_[Rgb32Float]		 = DXGI_FORMAT_R32G32B32_FLOAT;
		_[Rgb32UInt]		 = DXGI_FORMAT_R32G32B32_UINT;
		_[Rgb32SInt]		 = DXGI_FORMAT_R32G32B32_SINT;
		_[Rgba16Float]		 = DXGI_FORMAT_R16G16B16A16_FLOAT;
		_[Rgba16UNorm]		 = DXGI_FORMAT_R16G16B16A16_UNORM;
		_[Rgba16UInt]		 = DXGI_FORMAT_R16G16B16A16_UINT;
		_[Rgba16SNorm]		 = DXGI_FORMAT_R16G16B16A16_SNORM;
		_[Rgba16SInt]		 = DXGI_FORMAT_R16G16B16A16_SINT;
		_[Rg32Float]		 = DXGI_FORMAT_R32G32_FLOAT;
		_[Rg32UInt]			 = DXGI_FORMAT_R32G32_UINT;
		_[Rg32SInt]			 = DXGI_FORMAT_R32G32_SINT;
		_[R32G8X24Typeless]	 = DXGI_FORMAT_R32G8X24_TYPELESS;
		_[D32FloatS8X24UInt] = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		_[Rgb10A2UNorm]		 = DXGI_FORMAT_R10G10B10A2_UNORM;
		_[Rgb10A2UInt]		 = DXGI_FORMAT_R10G10B10A2_UINT;
		_[Rg11B10Float]		 = DXGI_FORMAT_R11G11B10_FLOAT;
		_[Rgba8UNorm]		 = DXGI_FORMAT_R8G8B8A8_UNORM;
		_[Rgba8UNormSrgb]	 = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		_[Rgba8UInt]		 = DXGI_FORMAT_R8G8B8A8_UINT;
		_[Rgba8SNorm]		 = DXGI_FORMAT_R8G8B8A8_SNORM;
		_[Rgba8SInt]		 = DXGI_FORMAT_R8G8B8A8_SINT;
		_[Bgra8UNorm]		 = DXGI_FORMAT_B8G8R8A8_UNORM;
		_[Bgra8UNormSrgb]	 = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		_[Rg16Float]		 = DXGI_FORMAT_R16G16_FLOAT;
		_[Rg16UNorm]		 = DXGI_FORMAT_R16G16_UNORM;
		_[Rg16UInt]			 = DXGI_FORMAT_R16G16_UINT;
		_[Rg16SNorm]		 = DXGI_FORMAT_R16G16_SNORM;
		_[Rg16SInt]			 = DXGI_FORMAT_R16G16_SINT;
		_[R32Typeless]		 = DXGI_FORMAT_R32_TYPELESS;
		_[D32Float]			 = DXGI_FORMAT_D32_FLOAT;
		_[R32Float]			 = DXGI_FORMAT_R32_FLOAT;
		_[R32UInt]			 = DXGI_FORMAT_R32_UINT;
		_[R32SInt]			 = DXGI_FORMAT_R32_SINT;
		_[R24G8Typeless]	 = DXGI_FORMAT_R24G8_TYPELESS;
		_[D24UNormS8UInt]	 = DXGI_FORMAT_D24_UNORM_S8_UINT;
		_[Rg8UNorm]			 = DXGI_FORMAT_R8G8_UNORM;
		_[Rg8UInt]			 = DXGI_FORMAT_R8G8_UINT;
		_[Rg8SNorm]			 = DXGI_FORMAT_R8G8_SNORM;
		_[Rg8SInt]			 = DXGI_FORMAT_R8G8_SINT;
		_[R16Typeless]		 = DXGI_FORMAT_R16_TYPELESS;
		_[R16Float]			 = DXGI_FORMAT_R16_FLOAT;
		_[D16UNorm]			 = DXGI_FORMAT_D16_UNORM;
		_[R16UNorm]			 = DXGI_FORMAT_R16_UNORM;
		_[R16UInt]			 = DXGI_FORMAT_R16_UINT;
		_[R16SNorm]			 = DXGI_FORMAT_R16_SNORM;
		_[R16SInt]			 = DXGI_FORMAT_R16_SINT;
		_[R8UNorm]			 = DXGI_FORMAT_R8_UNORM;
		_[R8UInt]			 = DXGI_FORMAT_R8_UINT;
		_[R8SNorm]			 = DXGI_FORMAT_R8_SNORM;
		_[R8SInt]			 = DXGI_FORMAT_R8_SINT;
		_[Bc1UNorm]			 = DXGI_FORMAT_BC1_UNORM;
		_[Bc1UNormSrgb]		 = DXGI_FORMAT_BC1_UNORM_SRGB;
		_[Bc2UNorm]			 = DXGI_FORMAT_BC2_UNORM;
		_[Bc2UNormSrgb]		 = DXGI_FORMAT_BC2_UNORM_SRGB;
		_[Bc3UNorm]			 = DXGI_FORMAT_BC3_UNORM;
		_[Bc3UNormSrgb]		 = DXGI_FORMAT_BC3_UNORM_SRGB;
		_[Bc4UNorm]			 = DXGI_FORMAT_BC4_UNORM;
		_[Bc4SNorm]			 = DXGI_FORMAT_BC4_SNORM;
		_[Bc5UNorm]			 = DXGI_FORMAT_BC5_UNORM;
		_[Bc5SNorm]			 = DXGI_FORMAT_BC5_SNORM;
		_[Bc6HUFloat16]		 = DXGI_FORMAT_BC6H_UF16;
		_[Bc6HSFloat16]		 = DXGI_FORMAT_BC6H_SF16;
		_[Bc7UNorm]			 = DXGI_FORMAT_BC7_UNORM;
		_[Bc7UNormSrgb]		 = DXGI_FORMAT_BC7_UNORM_SRGB;
		return _;
	}();

	export constexpr inline auto IMAGE_LAYOUT_LOOKUP = [] {
		LookupTable<ImageLayout, D3D12_RESOURCE_STATES> _;
		using enum ImageLayout;

		_[Undefined]			  = D3D12_RESOURCE_STATE_COMMON;
		_[General]				  = D3D12_RESOURCE_STATE_COMMON;
		_[CopySource]			  = D3D12_RESOURCE_STATE_COPY_SOURCE;
		_[CopyDestination]		  = D3D12_RESOURCE_STATE_COPY_DEST;
		_[ColorAttachment]		  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		_[DepthStencilAttachment] = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		_[DepthStencilReadOnly]	  = D3D12_RESOURCE_STATE_DEPTH_READ;
		_[ShaderResource]		  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		_[FragmentShaderResource] = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		_[UnorderedAccess]		  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		_[Presentable]			  = D3D12_RESOURCE_STATE_PRESENT;
		return _;
	}();

	constexpr inline auto IMAGE_DIMENSION_LOOKUP = [] {
		LookupTable<ImageDimension, D3D12_RESOURCE_DIMENSION> _;
		using enum ImageDimension;

		_[Image1D]	 = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		_[Image2D]	 = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		_[Image3D]	 = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		_[Cube]		 = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		_[Array1D]	 = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		_[Array2D]	 = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		_[CubeArray] = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		return _;
	}();

	D3D12_RESOURCE_FLAGS derive_image_resource_flags(ImageUsage usage)
	{
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		using enum ImageUsage;
		if(!(usage & Sampled))
			flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		if(usage & Storage || usage & InputAttachment)
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		if(usage & ColorAttachment)
			flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		if(usage & DepthStencil)
			flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		return flags;
	}

	D3D12_RESOURCE_STATES derive_image_resource_states(ImageUsage usage)
	{
		D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_COMMON;

		using enum ImageUsage;
		if(usage & CopySrc)
			states |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		if(usage & CopyDst)
			states |= D3D12_RESOURCE_STATE_COPY_DEST;
		if(usage & Sampled)
			states |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		if(usage & Storage || usage & InputAttachment)
			states |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		if(usage & ColorAttachment)
			states |= D3D12_RESOURCE_STATE_RENDER_TARGET;
		if(usage & DepthStencil)
			states |= D3D12_RESOURCE_STATE_DEPTH_WRITE;

		return states;
	}

	export class D3D12Image : public Resource
	{
	public:
		D3D12Image(ImageSpecification const& spec, D3D12MA::Allocator& allocator)
		{
			D3D12MA::ALLOCATION_DESC const allocation_desc {
				.HeapType = D3D12_HEAP_TYPE_DEFAULT,
			};
			D3D12_RESOURCE_DESC const resource_desc {
				.Dimension		  = IMAGE_DIMENSION_LOOKUP[spec.dimension],
				.Alignment		  = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
				.Width			  = spec.expanse.width,
				.Height			  = spec.expanse.height,
				.DepthOrArraySize = static_cast<UINT16>(spec.expanse.depth),
				.MipLevels		  = spec.mip_count,
				.Format			  = IMAGE_FORMAT_LOOKUP[spec.format],
				.SampleDesc {
					.Count	 = spec.sample_count,
					.Quality = 0,
				},
				.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
				.Flags	= derive_image_resource_flags(spec.usage),
			};
			auto initial_state = derive_image_resource_states(spec.usage);

			auto result = allocator.CreateResource(&allocation_desc, &resource_desc, initial_state, nullptr,
												   std::out_ptr(allocation), VT_COM_OUT(resource));
			VT_CHECK_RESULT(result, "Failed to create D3D12 image.");
		}
	};
}
