module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
module vt.Graphics.D3D12.Image;

import vt.Core.LookupTable;
import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.Resource;

namespace vt::d3d12
{
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

	D3D12_RESOURCE_DESC fill_resource_desc(ImageSpecification const& spec)
	{
		return {
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
	}

	D3D12Image::D3D12Image(ImageSpecification const& spec,
						   ID3D12Device4&			 device,
						   D3D12MA::Allocator&		 allocator,
						   DescriptorPool&			 pool)
	{
		initialize_resource(spec, allocator);
		initialize_descriptor(spec, device, pool);
	}

	void D3D12Image::initialize_resource(ImageSpecification const& spec, D3D12MA::Allocator& allocator)
	{
		D3D12MA::ALLOCATION_DESC const allocation_desc {
			.HeapType = D3D12_HEAP_TYPE_DEFAULT,
		};
		auto resource_desc = fill_resource_desc(spec);
		auto initial_state = derive_image_resource_states(spec.usage);

		auto result = allocator.CreateResource(&allocation_desc, &resource_desc, initial_state, nullptr,
											   std::out_ptr(allocation), VT_COM_OUT(resource));
		VT_CHECK_RESULT(result, "Failed to create D3D12 image.");
	}

	void D3D12Image::initialize_descriptor(ImageSpecification const& spec, ID3D12Device4& device, DescriptorPool& pool)
	{
		auto usage = spec.usage.get();
		using enum ImageUsage;

		if(usage & Storage || usage & InputAttachment)
		{
			descriptor = pool.allocate_cbv_srv_uav();
			device.CreateUnorderedAccessView(resource.get(), nullptr, nullptr, descriptor.get());
		}
		else if(usage & Sampled)
		{
			descriptor = pool.allocate_cbv_srv_uav();
			device.CreateShaderResourceView(resource.get(), nullptr, descriptor.get());
		}
	}
}
