module;
#include "Core/Macros.hpp"
export module vt.Graphics.SwapChain;

import vt.Graphics.DynamicGpuApi;
import vt.Graphics.SwapChainBase;
import vt.Graphics.VT_GPU_API_MODULE.SwapChain;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.SwapChain;
#endif

namespace vt
{
	export using PlatformSwapChain = InterfaceVariant<SwapChainBase, VT_GPU_API_VARIANT_ARGS(SwapChain)>;
	export class SwapChain : public PlatformSwapChain
	{
	public:
		static constexpr unsigned DEFAULT_BUFFERS = 3;

		using PlatformSwapChain::PlatformSwapChain;
	};
}
