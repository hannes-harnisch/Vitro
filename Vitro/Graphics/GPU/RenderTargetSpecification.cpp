export module vt.Graphics.RenderTargetSpecification;

import vt.Core.FixedList;
import vt.Graphics.AssetResource;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.SwapChain;

namespace vt
{
	export struct RenderTargetSpecification
	{
		FixedList<Texture const*, MaxColorAttachments> color_attachments;

		Texture const*	  depth_stencil_attachment = nullptr; // Optional depth stencil attachment.
		RenderPass const* render_pass			   = nullptr;
		SwapChain const*  swap_chain			   = nullptr; // Optional swap chain to use a color attachment from.
		unsigned		  swap_chain_src_index	   = -1;	  // Index of the swap chain back buffer to use as color attachment.
		unsigned		  swap_chain_dst_index	   = -1;	  // Index within the render target to put the back buffer.
	};
}
