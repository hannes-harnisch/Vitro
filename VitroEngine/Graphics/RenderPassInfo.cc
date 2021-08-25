export module vt.Graphics.RenderPassInfo;

import vt.Core.FixedList;
import vt.Graphics.TextureInfo;

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
		ImageFormat	  format		 = {};
		unsigned char sample_count	 = 1;
		ImageLoadOp	  load_op		 = {};
		ImageStoreOp  store_op		 = {};
		ImageLayout	  initial_layout = {};
		ImageLayout	  final_layout	 = {};
	};

	export struct AttachmentReference
	{
		unsigned char index		  = 0;
		ImageLayout	  used_layout = {};
	};

	export struct Subpass
	{
		FixedList<AttachmentReference, MaxColorAttachments> output_refs;
	};

	export struct RenderPassInfo
	{
		FixedList<AttachmentInfo, MaxAttachments> attachments;
		ImageLoadOp								  stencil_load_op  = {};
		ImageStoreOp							  stencil_store_op = {};
		FixedList<Subpass, MaxSubpasses>		  subpasses;
	};
}
