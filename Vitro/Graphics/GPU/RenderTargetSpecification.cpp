module;
#include <cstdint>
#include <stdexcept>
export module vt.Graphics.RenderTargetSpecification;

import vt.Core.Ref;
import vt.Core.FixedList;
import vt.Core.Specification;
import vt.Graphics.AssetResource;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.SwapChain;

namespace vt
{
	export struct RenderTargetSpecification
	{
		using ColorAttachmentList = FixedList<CRef<Image>, MAX_COLOR_ATTACHMENTS>;

		Explicit<ColorAttachmentList> color_attachments;
		Image const*				  depth_stencil_attachment = nullptr;
		RenderPass const&			  render_pass;
		Explicit<uint16_t>			  width;
		Explicit<uint16_t>			  height;
	};

	// Specifies render targets shared between renderers.
	export struct SharedRenderTargetSpecification
	{
		using ColorAttachmentList = FixedList<CRef<Image>, MAX_COLOR_ATTACHMENTS - 1>;

		ColorAttachmentList color_attachments;
		Image const*		depth_stencil_attachment = nullptr;
		RenderPass const&	render_pass;
		Explicit<unsigned>	shared_img_dst_index;
	};

	export void validate_render_target_spec(RenderTargetSpecification const& spec)
	{
		if(spec.color_attachments.empty())
			throw std::invalid_argument("A render target needs at least one color attachment.");
	}

	export void validate_shared_target_spec(SharedRenderTargetSpecification const& spec,
											SwapChain const&					   swap_chain,
											unsigned							   back_buffer_index)
	{
		if(back_buffer_index >= swap_chain->count_buffers())
			throw std::invalid_argument("Invalid swap chain back buffer index.");

		if(spec.shared_img_dst_index > spec.color_attachments.size())
			throw std::invalid_argument("Invalid swap chain image destination index.");
	}
}
