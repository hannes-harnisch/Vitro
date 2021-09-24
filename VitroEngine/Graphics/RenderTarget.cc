module;
#include "Core/Macros.hh"
export module vt.Graphics.RenderTarget;

import vt.Core.Array;
import vt.Graphics.AssetResource;
import vt.Graphics.Device;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.RenderPass;
import vt.Graphics.SwapChain;
import vt.VT_GPU_API_MODULE.RenderTarget;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.RenderTarget;
#endif

namespace vt
{
	using PlatformRenderTarget = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderTarget)>;
	export class RenderTarget : public PlatformRenderTarget
	{
	public:
		RenderTarget(Device const&			   device,
					 RenderPass const&		   pass,
					 ArrayView<Texture const*> color_attachments,
					 Texture const*			   depth_stencil_attachment = nullptr) :
			PlatformRenderTarget(device, pass, color_attachments, depth_stencil_attachment)
		{}

		RenderTarget(Device const& device, RenderPass const& pass, SwapChain const& swap_chain) :
			PlatformRenderTarget(device, pass, swap_chain)
		{}
	};
}
