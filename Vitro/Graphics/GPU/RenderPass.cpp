module;
#include "Core/Macros.hpp"
export module vt.Graphics.RenderPass;

import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.RenderPass;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.RenderPass;
#endif

namespace vt
{
	using PlatformRenderPass = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderPass)>;
	export class RenderPass : public PlatformRenderPass
	{
		using PlatformRenderPass::PlatformRenderPass;
	};
}
