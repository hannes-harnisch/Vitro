module;
#include "Core/Intrinsics.hh"
#include "D3D12.API.hh"
export module Vitro.D3D12.RenderPass;

import Vitro.Core.Array;
import Vitro.Graphics.Handle;
import Vitro.Graphics.RenderPassBase;

namespace vt::d3d12
{
	constexpr D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE convertAttachmentLoadOperation(AttachmentLoadOperation op)
	{
		switch(op)
		{
			case AttachmentLoadOperation::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
			case AttachmentLoadOperation::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
			case AttachmentLoadOperation::Ignore: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		}
		vtUnreachable();
	}

	constexpr D3D12_RENDER_PASS_ENDING_ACCESS_TYPE convertAttachmentStoreOperation(AttachmentStoreOperation op)
	{
		switch(op)
		{
			case AttachmentStoreOperation::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
			case AttachmentStoreOperation::Ignore: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		}
		vtUnreachable();
	}

	struct AttachmentInfo
	{
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE beginAccess = {};
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	endAccess	= {};
		D3D12_RESOURCE_STATES					beginLayout = {};
		D3D12_RESOURCE_STATES					endLayout	= {};
	};

	export class RenderPass : public RenderPassBase
	{
	public:
		RenderPass(DeviceHandle, RenderPassInfo const& info)
		{}

		unsigned char							colorAttachmentCount = 0;
		AttachmentInfo							colorAttachments[RenderPassBase::MaxColorAttachments];
		AttachmentInfo							depthAttachment;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE stencilBeginAccess = {};
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	stencilEndAccess   = {};
		unsigned char							subpassCount	   = 0;
	};
}
