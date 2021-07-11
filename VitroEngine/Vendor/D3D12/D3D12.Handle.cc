module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Handle;

import Vitro.D3D12.RenderPass;

namespace vt::d3d12
{
	export struct BufferHandle
	{
		ID3D12Resource* handle = nullptr;
	};

	export struct PipelineHandle
	{
		ID3D12PipelineState* handle = nullptr;
	};

	export struct RenderPassHandle
	{
		RenderPass* handle = nullptr;
	};

	export struct RenderTargetHandle
	{
		ID3D12Resource* handle = nullptr;
	};

	export struct TextureHandle
	{
		ID3D12Resource* handle = nullptr;
	};
}