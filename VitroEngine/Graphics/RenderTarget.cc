export module Vitro.Graphics.RenderTarget;

import Vitro.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.RenderTarget;
#endif
import Vitro.VT_GPU_API_MODULE.RenderTarget;

namespace vt
{
	export using RenderTarget = ResourceVariant<
#if VT_DYNAMIC_GPU_API
		VT_GPU_API_NAME_PRIMARY::RenderTarget,
#endif
		VT_GPU_API_NAME::RenderTarget>;
}
