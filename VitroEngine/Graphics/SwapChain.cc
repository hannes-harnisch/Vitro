export module Vitro.Graphics.SwapChain;

import Vitro.Graphics.DynamicGpuApi;
import Vitro.Graphics.SwapChainBase;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.SwapChain;
#endif
import Vitro.VT_GPU_API_MODULE.SwapChain;

namespace vt
{
	export using SwapChain = InterfaceVariant<SwapChainBase,
#if VT_DYNAMIC_GPU_API
											  VT_GPU_API_NAME_PRIMARY::SwapChain,
#endif
											  VT_GPU_API_NAME::SwapChain>;
}
