module;
#include "D3D12.API.hh"
export module Vitro.D3D12.RenderPass;

import Vitro.Core.Array;

namespace vt::d3d12
{
	export struct RenderPass
	{
		D3D12_RENDER_PASS_BEGINNING_ACCESS renderTargetBeginAccess;
		D3D12_RENDER_PASS_BEGINNING_ACCESS depthBeginAccess;
		D3D12_RENDER_PASS_BEGINNING_ACCESS stencilBeginAccess;
		D3D12_RENDER_PASS_ENDING_ACCESS renderTargetEndAccess;
		D3D12_RENDER_PASS_ENDING_ACCESS depthEndAccess;
		D3D12_RENDER_PASS_ENDING_ACCESS stencilEndAccess;
		D3D12_RENDER_PASS_FLAGS flags;
		Array<D3D12_RESOURCE_BARRIER> subpassTransitionBarriers;
	};
}
