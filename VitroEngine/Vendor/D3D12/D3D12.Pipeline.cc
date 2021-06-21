module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Pipeline;

namespace D3D12
{
	export struct Pipeline
	{
		ID3D12PipelineState* handle;
	};
}
