export module Vitro.Graphics.RenderPassInfo;

import Vitro.Core.FixedList;
import Vitro.Graphics.TextureInfo;

namespace vt
{
	export constexpr unsigned MaxColorAttachments = 8;						 // limit imposed by D3D12 for pipelines
	export constexpr unsigned MaxAttachments	  = MaxColorAttachments + 1; // All color attachments + 1 depth attachment
	export constexpr unsigned MaxSubpasses		  = 5;						 // arbitrary limit, can be changed later

	export enum class ImageLoadOp : unsigned char {
		Load,
		Clear,
		Ignore,
	};

	export enum class ImageStoreOp : unsigned char {
		Store,
		Ignore,
	};

	export struct AttachmentInfo
	{
		ImageFormat	  format		= {};
		unsigned char sampleCount	= 1;
		ImageLoadOp	  loadOp		= {};
		ImageStoreOp  storeOp		= {};
		ImageLayout	  initialLayout = {};
		ImageLayout	  finalLayout	= {};
	};

	export struct AttachmentReference
	{
		unsigned char index		 = 0;
		ImageLayout	  usedLayout = {};
	};

	export struct Subpass
	{
		FixedList<AttachmentReference, MaxColorAttachments> outputRefs;
	};

	export struct RenderPassInfo
	{
		FixedList<AttachmentInfo, MaxAttachments> attachments;
		ImageLoadOp								  stencilLoadOp	 = {};
		ImageStoreOp							  stencilStoreOp = {};
		FixedList<Subpass, MaxSubpasses>		  subpasses;
	};
}
