module;
#include "Core/Macros.hpp"

#include <stdexcept>
export module vt.Graphics.RenderTarget;

import vt.Core.Array;
import vt.Graphics.AssetResource;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.SwapChain;
import vt.Graphics.VT_GPU_API_MODULE.RenderTarget;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.RenderTarget;
#endif

namespace vt
{
	using PlatformRenderTarget = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderTarget)>;
	export class RenderTarget : public PlatformRenderTarget
	{
		using PlatformRenderTarget::PlatformRenderTarget;

	public:
		static void validate_spec(RenderTargetSpecification const& spec)
		{
			if(!spec.render_pass)
				throw std::invalid_argument("Render pass must not be nullptr.");

			if(spec.swap_chain)
			{
				if(spec.swap_chain_src_index >= (*spec.swap_chain)->get_buffer_count())
					throw std::invalid_argument("Invalid swap chain image source index.");

				if(spec.swap_chain_dst_index > spec.color_attachments.size())
					throw std::invalid_argument("Invalid swap chain image destination index.");
			}
			else if(spec.color_attachments.empty())
				throw std::invalid_argument("A render target needs at least one color attachment.");

			for(auto attachment : spec.color_attachments)
				if(!attachment)
					throw std::invalid_argument("Attachments must not be nullptr.");

			if(spec.color_attachments.full() && spec.swap_chain)
				throw std::invalid_argument("A swap chain must not be provided if the maximum of possible color attachments is "
											"already specified.");
		}
	};
}
