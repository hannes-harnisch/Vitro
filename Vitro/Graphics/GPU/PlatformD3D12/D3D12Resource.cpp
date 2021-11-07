module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Resource;

import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.D3D12.Handle;

namespace vt::d3d12
{
	export class Resource
	{
	public:
		void* map() const
		{
			void* ptr;
			auto  result = resource->Map(0, nullptr, &ptr);
			VT_CHECK_RESULT(result, "Failed to map D3D12 resource.");
			return ptr;
		}

		void unmap() const
		{
			resource->Unmap(0, nullptr);
		}

		D3D12_GPU_VIRTUAL_ADDRESS get_gpu_address() const
		{
			return resource->GetGPUVirtualAddress();
		}

		ID3D12Resource* get_resource() const
		{
			return resource.get();
		}

	protected:
		ComUnique<D3D12MA::Allocation> allocation;
		ComUnique<ID3D12Resource>	   resource;
	};
}
