export module Vitro.Graphics.RenderPassBase;

import Vitro.Core.FixedList;
import Vitro.Graphics.TextureInfo;

namespace vt
{
	export constexpr unsigned MaxColorAttachments = 8;						 // limit imposed by D3D12 for pipelines
	export constexpr unsigned MaxAttachments	  = MaxColorAttachments + 1; // All color attachments + 1 depth attachment
	export constexpr unsigned MaxSubpasses		  = 5;						 // arbitrary limit, can be changed later

	export class RenderPassBase
	{
	public:
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
		ImageFormat				 format		   = {};
		unsigned char			 sampleCount   = 1;
		AttachmentLoadOperation	 loadOp		   = {};
		AttachmentStoreOperation storeOp	   = {};
		ImageLayout				 initialLayout = {};
		ImageLayout				 finalLayout   = {};
	};

	export struct AttachmentReference
	{
		unsigned char index		 = 0;
		ImageLayout	  usedLayout = {};
	};

	export struct Subpass
	{
		FixedList<AttachmentReference, MaxColorAttachments> refs;
	};

	export struct RenderPassInfo
	{
		FixedList<AttachmentInfo, MaxAttachments> attachments;
		AttachmentLoadOperation					  stencilLoadOp	 = {};
		AttachmentStoreOperation				  stencilStoreOp = {};
		FixedList<Subpass, MaxSubpasses>		  subpasses;
	};
}
