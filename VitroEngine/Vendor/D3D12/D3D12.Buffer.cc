module;
#include "D3D12.API.hh"
export module vt.D3D12.Buffer;

import vt.D3D12.Utils;
import vt.Graphics.Device;

namespace vt::d3d12
{
	export class D3D12Buffer
	{
	public:
		D3D12_GPU_VIRTUAL_ADDRESS get_gpu_address() const
		{
			return buffer->GetGPUVirtualAddress();
		}

		unsigned get_size() const
		{
			return size;
		}

		unsigned get_stride() const
		{
			return stride;
		}

	private:
		ComUnique<ID3D12Resource> buffer;
		unsigned				  size	 = 0;
		unsigned				  stride = 0;
	};
}
