module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Resource;

namespace D3D12
{
	export struct Buffer
	{
		ID3D12Resource* handle {};
	};

	export struct Pipeline
	{
		ID3D12PipelineState* handle {};
	};

	export struct Texture
	{
		ID3D12Resource* handle {};
	};
}
