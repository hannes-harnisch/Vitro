export module vt.Graphics.SwapChain;

import vt.Graphics.DynamicGpuApi;
import vt.Graphics.SwapChainBase;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.SwapChain;
#endif
import vt.VT_GPU_API_MODULE.SwapChain;

namespace vt
{
	export using SwapChain = InterfaceVariant<SwapChainBase,
#if VT_DYNAMIC_GPU_API
											  VT_GPU_API_NAME_PRIMARY::SwapChain,
#endif
											  VT_GPU_API_NAME::SwapChain>;
}
