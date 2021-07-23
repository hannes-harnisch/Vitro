module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Handle;

import Vitro.D3D12.RenderPass;

namespace vt::d3d12
{
	export struct PlatformBufferHandle
	{
		ID3D12Resource* handle = nullptr;
	};

	export struct PlatformCommandListHandle
	{
		ID3D12CommandList* handle = nullptr;
	};

	export struct PlatformPipelineHandle
	{
		ID3D12PipelineState* handle = nullptr;
	};

	export struct PlatformRenderPassHandle
	{
		RenderPass* handle = nullptr;
	};

	export struct PlatformRenderTargetHandle
	{
		ID3D12Resource* resource		= nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE rtv = {};
		D3D12_CPU_DESCRIPTOR_HANDLE dsv = {};
	};

	export struct PlatformRootSignatureHandle
	{
		ID3D12RootSignature* handle = nullptr;
	};

	export struct PlatformTextureHandle
	{
		ID3D12Resource* handle = nullptr;
	};
}
