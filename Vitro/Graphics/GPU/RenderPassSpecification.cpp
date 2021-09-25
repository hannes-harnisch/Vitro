module;
#include <cstdint>
export module vt.Graphics.RenderPassSpecification;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.Specification;
import vt.Graphics.TextureSpecification;

namespace vt
{
	export constexpr unsigned MaxColorAttachments = 8;						 // Limit imposed by D3D12 for pipelines
	export constexpr unsigned MaxAttachments	  = MaxColorAttachments + 1; // All color attachments + 1 depth attachment

	export enum class ImageLoadOp : uint8_t {
		Load,
		Clear,
		Ignore,
	};

	export enum class ImageStoreOp : uint8_t {
		Store,
		Ignore,
	};

	export struct AttachmentReference
	{
		Explicit<uint8_t>	  index;
		Explicit<ImageLayout> used_layout;
	};

	export struct Subpass
	{
		Explicit<FixedList<AttachmentReference, MaxColorAttachments>> output_refs;
	};

	export struct AttachmentSpecification
	{
		Explicit<ImageFormat>  format;
		uint8_t				   sample_count = 1;
		Explicit<ImageLoadOp>  load_op;
		Explicit<ImageStoreOp> store_op;
		Explicit<ImageLayout>  initial_layout;
		Explicit<ImageLayout>  final_layout;
	};

	export struct RenderPassSpecification
	{
		Explicit<FixedList<AttachmentSpecification, MaxAttachments>> attachments;

		ImageLoadOp		   stencil_load_op	= ImageLoadOp::Ignore;
		ImageStoreOp	   stencil_store_op = ImageStoreOp::Ignore;
		ArrayView<Subpass> subpasses;
	};
}
