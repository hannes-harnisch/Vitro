module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"
export module vt.D3D12.DescriptorPool;

import vt.D3D12.Utils;
import vt.Graphics.Device;

namespace vt::d3d12
{
	export class D3D12DescriptorPool
	{
	public:
		D3D12DescriptorPool(Device const& in_device)
		{
			auto device = in_device.d3d12.ptr();

			D3D12_DESCRIPTOR_HEAP_DESC const view_heap_desc {
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			};
		}

	private:
		ComUnique<ID3D12DescriptorHeap> view_heap;
		ComUnique<ID3D12DescriptorHeap> sampler_heap;
	};
}
