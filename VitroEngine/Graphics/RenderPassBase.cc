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

	struct AttachmentInfo
	{
		ImageFormat				 format		 = {};
		unsigned char			 sampleCount = 1;
		AttachmentLoadOperation	 loadOp		 = {};
		AttachmentStoreOperation storeOp	 = {};
		ImageLayout				 startLayout = {};
		ImageLayout				 endLayout	 = {};
	};
	
	struct AttachmentTransition
	{
		unsigned char attachmentIndex = 0;
		ImageLayout	  targetLayout	  = {};
	};

	struct SubpassInfo
	{
		static constexpr unsigned MaxAttachmentsInGroup = RenderPassBase::MaxColorAttachments - 1;
		
		ImageLayout			 depthStencilLayout	  = {};
		unsigned char		 inputAttachmentCount = 0;
		AttachmentTransition inputAttachments[MaxAttachmentsInGroup];
		unsigned char		 nonInputAttachmentCount = 0;
		AttachmentTransition nonInputAttachments[MaxAttachmentsInGroup];
	};

	export struct RenderPassInfo
	{
		unsigned char			 colorAttachmentCount = 0;
		AttachmentInfo			 colorAttachments[RenderPassBase::MaxColorAttachments];
		AttachmentInfo			 depthAttachment;
		AttachmentLoadOperation	 stencilLoadOp	= {};
		AttachmentStoreOperation stencilStoreOp = {};
		unsigned char			 subpassCount = 0;
		SubpassInfo				 subpasses[RenderPassBase::MaxSubpasses];
	};
}
