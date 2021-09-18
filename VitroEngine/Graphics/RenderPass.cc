module;
#include "Core/Macros.hh"
export module vt.Graphics.RenderPass;

import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.RenderPass;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.RenderPass;
#endif

namespace vt
{
	export using RenderPass = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderPass)>;
}
