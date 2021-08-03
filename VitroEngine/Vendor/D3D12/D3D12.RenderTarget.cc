module;
#include "D3D12.API.hh"
export module Vitro.D3D12.RenderTarget;

import Vitro.D3D12.Utils;
import Vitro.Graphics.RenderPassBase;
import Vitro.Graphics.RenderTargetBase;

namespace vt::d3d12
{
	export class RenderTarget : public RenderTargetBase
	{
	public:
		UniqueInterface<ID3D12Resource> colorAttachments[RenderPassBase::MaxColorAttachments];
		D3D12_CPU_DESCRIPTOR_HANDLE		colorAttachmentViews[RenderPassBase::MaxColorAttachments];
		unsigned						colorAttachmentCount = 0;
		UniqueInterface<ID3D12Resource> depthStencilAttachment;
		D3D12_CPU_DESCRIPTOR_HANDLE		depthStencilView;
	};
}
