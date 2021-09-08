module;
#include "Core/Macros.hh"
export module vt.Graphics.RenderTarget;

import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.RenderTarget;
#endif
import vt.VT_GPU_API_MODULE.RenderTarget;

namespace vt
{
	export using RenderTarget = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderTarget)>;
}
