module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Resource;

namespace vt::d3d12
{
	export struct Buffer
	{
		ID3D12Resource* handle = nullptr;
	};

	export struct Pipeline
	{
		ID3D12PipelineState* handle = nullptr;
	};

	export struct Texture
	{
		ID3D12Resource* handle = nullptr;
	};
}
