module;
#include "D3D12.API.hh"
export module Vitro.D3D12.RootSignature;

import Vitro.D3D12.Utils;
import Vitro.Graphics.Device;

namespace vt::d3d12
{
	export class RootSignature
	{
	public:
		ID3D12RootSignature* get() const
		{
			return rootSignature.get();
		}

	private:
		ComUnique<ID3D12RootSignature> rootSignature;
	};
}
