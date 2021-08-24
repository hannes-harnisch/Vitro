export module Vitro.Graphics.RenderPass;

import Vitro.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.RenderPass;
#endif
import Vitro.VT_GPU_API_MODULE.RenderPass;

namespace vt
{
	export using RenderPass = ResourceVariant<
#if VT_DYNAMIC_GPU_API
		VT_GPU_API_NAME_PRIMARY::RenderPass,
#endif
		VT_GPU_API_NAME::RenderPass>;
}
