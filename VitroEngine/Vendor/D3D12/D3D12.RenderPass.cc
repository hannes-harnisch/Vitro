module;
#include "D3D12.API.hh"
export module Vitro.D3D12.RenderPass;

import Vitro.Core.Array;

namespace vt::d3d12
{
	export struct RenderPass
	{
		D3D12_RENDER_PASS_RENDER_TARGET_DESC renderTargetDesc;
		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D12_RENDER_PASS_FLAGS flags;
		Array<D3D12_RESOURCE_BARRIER> subpassTransitionBarriers;
	};
}
