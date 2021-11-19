module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
module vt.Graphics.D3D12.Resource;

import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.D3D12.DescriptorAllocator;
import vt.Graphics.D3D12.Handle;

namespace vt::d3d12
{
	void* Resource::map() const
	{
		void* ptr;
		auto  result = resource->Map(0, nullptr, &ptr);
		VT_CHECK_RESULT(result, "Failed to map D3D12 resource.");
		return ptr;
	}

	void Resource::unmap() const
	{
		resource->Unmap(0, nullptr);
	}

	D3D12_GPU_VIRTUAL_ADDRESS Resource::get_gpu_address() const
	{
		return resource->GetGPUVirtualAddress();
	}

	ID3D12Resource* Resource::get_resource() const
	{
		return resource.get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Resource::get_descriptor() const
	{
		return descriptor.get();
	}
}
