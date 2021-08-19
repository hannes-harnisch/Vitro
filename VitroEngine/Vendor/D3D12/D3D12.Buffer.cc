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
		ID3D12Resource* get() const
		{
			return buffer.get();
		}

	private:
		ComUnique<ID3D12Resource> buffer;
	};
}
