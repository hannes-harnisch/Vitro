module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Pipeline;

import Vitro.D3D12.Resource;
import Vitro.Graphics.DynamicGraphicsAPI;

namespace D3D12
{
	export struct Pipeline : Resource<Pipeline>
	{
		ID3D12PipelineState* handle;
	};
}
