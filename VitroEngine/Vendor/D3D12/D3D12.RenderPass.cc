module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <optional>
export module Vitro.D3D12.RenderPass;

import Vitro.D3D12.Texture;
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
		DXGI_FORMAT								format;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE beginAccess;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	endAccess;
		D3D12_RESOURCE_STATES					startLayout;
		D3D12_RESOURCE_STATES					endLayout;
	};

	constexpr AttachmentInfo convertAttachmentInfo(vt::AttachmentInfo info)
	{
		return {
			.format		 = convertImageFormat(info.format),
			.beginAccess = convertAttachmentLoadOperation(info.loadOp),
			.endAccess	 = convertAttachmentStoreOperation(info.storeOp),
			.startLayout = convertImageLayout(info.startLayout),
			.endLayout	 = convertImageLayout(info.endLayout),
		};
	}

	struct AttachmentTransition
	{
		unsigned			  attachmentIndex;
		D3D12_RESOURCE_STATES targetLayout;
	};

	struct Subpass
	{
		AttachmentTransition				 colorTransitions[vt::Subpass::MaxAttachmentsInGroup];
		unsigned							 transitionCount;
		std::optional<D3D12_RESOURCE_STATES> depthStencilLayout;
	};

	constexpr AttachmentTransition convertAttachmentTransition(vt::AttachmentTransition transition)
	{
		return {
			.attachmentIndex = transition.attachmentIndex,
			.targetLayout	 = convertImageLayout(transition.targetLayout),
		};
	}

	constexpr Subpass convertSubpass(vt::Subpass subpass)
	{
		Subpass converted;
		if(subpass.depthStencilLayout.has_value())
			converted.depthStencilLayout = convertImageLayout(*subpass.depthStencilLayout);

		converted.transitionCount = subpass.inputTransitionCount + subpass.nonInputTransitionCount;

		for(unsigned i = 0; i < subpass.inputTransitionCount; ++i)
			converted.colorTransitions[i] = convertAttachmentTransition(subpass.inputTransitions[i]);

		for(unsigned i = subpass.inputTransitionCount; i < converted.transitionCount; ++i)
			converted.colorTransitions[i] = convertAttachmentTransition(subpass.nonInputTransitions[i]);

		return converted;
	}

	export class RenderPass : public RenderPassBase
	{
	public:
		RenderPass(DeviceHandle, RenderPassInfo const& info) :
			colorAttachmentCount(info.colorAttachmentCount),
			depthStencilAttachment(convertAttachmentInfo(info.depthStencilAttachment)),
			stencilBeginAccess(convertAttachmentLoadOperation(info.stencilLoadOp)),
			stencilEndAccess(convertAttachmentStoreOperation(info.stencilStoreOp)),
			subpassCount(info.subpassCount)
		{
			vtAssertPure(colorAttachmentCount <= RenderPassBase::MaxColorAttachments,
						 "Too many color attachments specified for the render pass.");

			for(unsigned i = 0; i < colorAttachmentCount; ++i)
				colorAttachments[i] = convertAttachmentInfo(info.colorAttachments[i]);

			for(unsigned i = 0; i < subpassCount; ++i)
			{
				int totalAttachments = info.subpasses[i].inputTransitionCount + info.subpasses[i].nonInputTransitionCount;
				vtAssertPure(totalAttachments <= RenderPassBase::MaxColorAttachments,
							 "This subpass specified too many attachments.");

				subpasses[i] = convertSubpass(info.subpasses[i]);
			}
		}

		AttachmentInfo							colorAttachments[RenderPassBase::MaxColorAttachments];
		unsigned char							colorAttachmentCount;
		AttachmentInfo							depthStencilAttachment;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE stencilBeginAccess;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	stencilEndAccess;
		Subpass									subpasses[RenderPassBase::MaxSubpasses];
		unsigned char							subpassCount;
	};
}
