module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Buffer;

import Vitro.D3D12.Utils;
import Vitro.Graphics.Device;

namespace vt::d3d12
{
	export class Buffer
	{
	public:
		D3D12_GPU_VIRTUAL_ADDRESS getGpuAddress() const
		{
			return buffer->GetGPUVirtualAddress();
		}

		unsigned getSize() const
		{
			return size;
		}

		unsigned getStride() const
		{
			return stride;
		}

	private:
		ComUnique<ID3D12Resource> buffer;
		unsigned				  size	 = 0;
		unsigned				  stride = 0;
	};
}
