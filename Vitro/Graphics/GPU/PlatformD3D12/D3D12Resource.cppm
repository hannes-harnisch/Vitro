module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Resource;

import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.D3D12.DescriptorAllocator;
import vt.Graphics.D3D12.Handle;

namespace vt::d3d12
{
	export class Resource
	{
	public:
		void*						map() const;
		void						unmap() const;
		D3D12_GPU_VIRTUAL_ADDRESS	get_gpu_address() const;
		ID3D12Resource*				get_resource() const;
		D3D12_CPU_DESCRIPTOR_HANDLE get_descriptor() const;

	protected:
		ComUnique<D3D12MA::Allocation> allocation;
		ComUnique<ID3D12Resource>	   resource;
		UniqueCpuDescriptor			   descriptor;
	};
}
