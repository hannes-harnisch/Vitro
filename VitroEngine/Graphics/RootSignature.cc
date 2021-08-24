export module Vitro.Graphics.RootSignature;

import Vitro.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.RootSignature;
#endif
import Vitro.VT_GPU_API_MODULE.RootSignature;

namespace vt
{
	export using RootSignature = ResourceVariant<
#if VT_DYNAMIC_GPU_API
		VT_GPU_API_NAME_PRIMARY::RootSignature,
#endif
		VT_GPU_API_NAME::RootSignature>;
}
