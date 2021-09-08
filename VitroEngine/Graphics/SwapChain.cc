module;
#include "Core/Macros.hh"
export module vt.Graphics.SwapChain;

import vt.Graphics.DynamicGpuApi;
import vt.Graphics.SwapChainBase;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.SwapChain;
#endif
import vt.VT_GPU_API_MODULE.SwapChain;

namespace vt
{
	export using SwapChain = InterfaceVariant<SwapChainBase, VT_GPU_API_VARIANT_ARGS(SwapChain)>;
}
