module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
export module vt.Graphics.D3D12.Buffer;

import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.D3D12.Resource;

namespace vt::d3d12
{
	export class D3D12Buffer : public Resource
	{
	public:
		D3D12Buffer(BufferSpecification const& spec, D3D12MA::Allocator& allocator)
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
			auto result = allocator.CreateResource(&allocation_desc, &resource_desc, initial_state, nullptr,
												   std::out_ptr(allocation), VT_COM_OUT(resource));
			VT_CHECK_RESULT(result, "Failed to create D3D12 buffer.");
		}
	};
}
