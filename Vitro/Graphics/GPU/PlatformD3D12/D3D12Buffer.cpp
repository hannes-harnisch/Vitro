module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Buffer;

import vt.Graphics.D3D12.Handle;
import vt.Graphics.Device;

namespace vt::d3d12
{
	export class D3D12Buffer
	{
	public:
		D3D12_GPU_VIRTUAL_ADDRESS get_gpu_address() const
		{
			return buffer->GetResource()->GetGPUVirtualAddress();
		}

	private:
		ComUnique<D3D12MA::Allocation> buffer;
	};
}
