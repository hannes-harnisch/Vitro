export module vt.Graphics.RenderTarget;

import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.RenderTarget;
#endif
import vt.VT_GPU_API_MODULE.RenderTarget;

namespace vt
{
	export using RenderTarget = ResourceVariant<
#if VT_DYNAMIC_GPU_API
		VT_GPU_API_NAME_PRIMARY::RenderTarget,
#endif
		VT_GPU_API_NAME::RenderTarget>;
}
