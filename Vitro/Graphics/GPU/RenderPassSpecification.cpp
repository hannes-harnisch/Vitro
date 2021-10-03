module;
#include <cstdint>
export module vt.Graphics.RenderPassSpecification;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.Specification;
import vt.Graphics.TextureSpecification;

namespace vt
{
	export constexpr inline unsigned MAX_COLOR_ATTACHMENTS = 8;					  // Limit imposed by D3D12 for pipelines
	export constexpr inline unsigned MAX_ATTACHMENTS = MAX_COLOR_ATTACHMENTS + 1; // All color attachments + 1 depth attachment

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
		using AttachmentRefList = FixedList<AttachmentReference, MAX_COLOR_ATTACHMENTS>;

		// Explicit<AttachmentRefList> input_refs; TODO: integrate this eventually
		Explicit<AttachmentRefList> output_refs;
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
		using AttachmentList = FixedList<AttachmentSpecification, MAX_ATTACHMENTS>;

		Explicit<AttachmentList> attachments;
		ImageLoadOp				 stencil_load_op  = ImageLoadOp::Ignore;
		ImageStoreOp			 stencil_store_op = ImageStoreOp::Ignore;
		ArrayView<Subpass>		 subpasses;
	};
}
