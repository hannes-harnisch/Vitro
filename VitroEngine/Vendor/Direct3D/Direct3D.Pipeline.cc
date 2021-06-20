module;
#include "Direct3D.API.hh"
export module Vitro.Direct3D.Pipeline;

namespace Direct3D
{
	export struct Pipeline
	{
		ID3D12PipelineState* handle;
	};
}
