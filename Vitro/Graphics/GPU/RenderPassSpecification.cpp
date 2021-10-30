module;
#include <algorithm>
#include <cstdint>
#include <stdexcept>
export module vt.Graphics.RenderPassSpecification;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.Specification;
import vt.Graphics.AssetResourceSpecification;

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

	export struct Subpass
	{
		using AttachmentIndexList = FixedList<uint8_t, MAX_COLOR_ATTACHMENTS>;

		AttachmentIndexList			  input_attachments;
		Explicit<AttachmentIndexList> output_attachments;
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

		bool uses_depth_stencil_attachment() const
		{
			auto uses_depth_stencil_layout = [](auto attachment) {
				return attachment.final_layout == ImageLayout::DepthStencilAttachment ||
					   attachment.final_layout == ImageLayout::DepthStencilReadOnly;
			};
			auto it = std::find_if(attachments.begin(), attachments.end(), uses_depth_stencil_layout);

			auto second = std::find_if(it, attachments.end(), uses_depth_stencil_layout);
			if(second != attachments.end())
				throw std::invalid_argument("Multiple depth stencil attachments are not allowed.");

			return it != attachments.end();
		}
	};
}
