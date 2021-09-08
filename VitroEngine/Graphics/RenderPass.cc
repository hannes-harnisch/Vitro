module;
#include "Core/Macros.hh"
export module vt.Graphics.RenderPass;

import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.RenderPass;
#endif
import vt.VT_GPU_API_MODULE.RenderPass;

namespace vt
{
	export using RenderPass = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderPass)>;
}
