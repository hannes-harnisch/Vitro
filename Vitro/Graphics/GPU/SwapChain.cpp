module;
#include "Core/Macros.hpp"
export module vt.Graphics.SwapChain;

import vt.App.ObjectEvent;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.SwapChainBase;
import vt.Graphics.VT_GPU_API_MODULE.SwapChain;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.SwapChain;
#endif

namespace vt
{
	using PlatformSwapChain = InterfaceVariant<SwapChainBase, VT_GPU_API_VARIANT_ARGS(SwapChain)>;
	export class SwapChain : public PlatformSwapChain, public ObjectEventSentinel<SwapChain>
	{
	public:
		static constexpr unsigned DefaultBufferCount = 3;

		SwapChain(Driver const& driver, Device& device, Window& window, uint8_t buffer_count = DefaultBufferCount) :
			PlatformSwapChain(driver, device, window, buffer_count)
		{}
	};
}
