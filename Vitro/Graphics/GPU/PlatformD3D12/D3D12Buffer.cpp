module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Buffer;

import vt.Graphics.D3D12.Handle;

namespace vt::d3d12
{
	export class D3D12Buffer
	{
	public:
		D3D12Buffer(D3D12MA::Allocator*)
		{}

		D3D12_GPU_VIRTUAL_ADDRESS get_gpu_address() const
		{
			return buffer->GetGPUVirtualAddress();
		}

	private:
		ComUnique<D3D12MA::Allocation> allocation;
		ComUnique<ID3D12Resource>	   buffer;
	};
}
