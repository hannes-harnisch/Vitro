module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
export module vt.Graphics.D3D12.Buffer;

import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.Resource;

namespace vt::d3d12
{
	export class D3D12Buffer : public Resource
	{
	public:
		D3D12Buffer(BufferSpecification const& spec, ID3D12Device4& device, D3D12MA::Allocator& allocator, DescriptorPool& pool)
		{
			initialize_resource(spec, allocator);
			initialize_descriptor(spec, device, pool);
		}

	private:
		static D3D12_RESOURCE_DESC fill_resource_desc(BufferSpecification const& spec)
		{
			return {
				.Dimension		  = D3D12_RESOURCE_DIMENSION_BUFFER,
				.Alignment		  = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
				.Width			  = spec.size,
				.Height			  = 1,
				.DepthOrArraySize = 1,
				.MipLevels		  = 1,
				.Format			  = DXGI_FORMAT_UNKNOWN,
				.SampleDesc {
					.Count	 = 1,
					.Quality = 0,
				},
				.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
				.Flags	= D3D12_RESOURCE_FLAG_NONE,
			};
		}

		void initialize_resource(BufferSpecification const& spec, D3D12MA::Allocator& allocator)
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
			auto resource_desc = fill_resource_desc(spec);
			auto result		   = allocator.CreateResource(&allocation_desc, &resource_desc, initial_state, nullptr,
													  std::out_ptr(allocation), VT_COM_OUT(resource));
			VT_CHECK_RESULT(result, "Failed to create D3D12 buffer.");
		}

		void initialize_descriptor(BufferSpecification const& spec, ID3D12Device4& device, DescriptorPool& pool)
		{
			auto usage = spec.usage.get();
			using enum BufferUsage;

			if(usage & RwUntyped || usage & Storage)
			{
				descriptor = pool.allocate_cbv_srv_uav();
				device.CreateUnorderedAccessView(resource.get(), nullptr, nullptr, descriptor.get());
			}
			else if(usage & Uniform)
			{
				descriptor = pool.allocate_cbv_srv_uav();
				D3D12_CONSTANT_BUFFER_VIEW_DESC const desc {
					.BufferLocation = resource->GetGPUVirtualAddress(),
					.SizeInBytes	= static_cast<UINT>(spec.size),
				};
				device.CreateConstantBufferView(&desc, descriptor.get());
			}
			else if(usage & Untyped)
			{
				descriptor = pool.allocate_cbv_srv_uav();
				device.CreateShaderResourceView(resource.get(), nullptr, descriptor.get());
			}
		}
	};
}
