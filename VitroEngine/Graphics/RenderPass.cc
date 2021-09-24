module;
#include "Core/Macros.hh"
export module vt.Graphics.RenderPass;

import vt.Graphics.Device;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.TextureSpecification;
import vt.VT_GPU_API_MODULE.RenderPass;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.RenderPass;
#endif

namespace vt
{
	using PlatformRenderPass = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderPass)>;
	export class RenderPass : public PlatformRenderPass
	{
	public:
		RenderPass(Device const& device, RenderPassSpecification const& spec) : PlatformRenderPass(device, spec)
		{}
	};
}
