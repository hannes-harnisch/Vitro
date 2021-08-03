module;
#include <optional>
export module Vitro.Graphics.RenderPassBase;

import Vitro.Graphics.TextureInfo;

namespace vt
{
	export class RenderPassBase
	{
	public:
		static constexpr unsigned MaxColorAttachments = 8; // limit imposed by D3D12 for pipelines
		static constexpr unsigned MaxSubpasses		  = 5; // arbitrary limit, can be changed later

		virtual ~RenderPassBase() = default;
	};

	export enum class AttachmentLoadOperation : unsigned char {
		Load,
		Clear,
		Ignore,
	};

	export enum class AttachmentStoreOperation : unsigned char {
		Store,
		Ignore,
	};

	export struct AttachmentInfo
	{
		ImageFormat				 format		 = {};
		unsigned char			 sampleCount = 1;
		AttachmentLoadOperation	 loadOp		 = {};
		AttachmentStoreOperation storeOp	 = {};
		ImageLayout				 startLayout = {};
		ImageLayout				 endLayout	 = {};
	};

	export struct AttachmentTransition
	{
		unsigned char attachmentIndex = 0;
		ImageLayout	  targetLayout	  = {};
	};

	export struct Subpass
	{
		static constexpr unsigned MaxAttachmentsInGroup = RenderPassBase::MaxColorAttachments - 1;

		AttachmentTransition	   inputTransitions[MaxAttachmentsInGroup];
		unsigned char			   inputTransitionCount = 0;
		AttachmentTransition	   nonInputTransitions[MaxAttachmentsInGroup];
		unsigned char			   nonInputTransitionCount = 0;
		std::optional<ImageLayout> depthStencilLayout;
	};

	export struct RenderPassInfo
	{
		AttachmentInfo			 colorAttachments[RenderPassBase::MaxColorAttachments];
		unsigned char			 colorAttachmentCount = 0;
		AttachmentInfo			 depthStencilAttachment;
		AttachmentLoadOperation	 stencilLoadOp	= {};
		AttachmentStoreOperation stencilStoreOp = {};
		Subpass					 subpasses[RenderPassBase::MaxSubpasses];
		unsigned char			 subpassCount = 0;
	};
}
