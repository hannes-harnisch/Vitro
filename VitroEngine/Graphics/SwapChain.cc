export module Vitro.Graphics.SwapChain;

import Vitro.Graphics.InterfaceVariant;
import Vitro.Graphics.SwapChainBase;

#if VT_DYNAMIC_GRAPHICS_API
import Vitro.VT_GRAPHICS_API_MODULE_PRIMARY.SwapChain;
#endif
import Vitro.VT_GRAPHICS_API_MODULE.SwapChain;

namespace vt
{
	export using SwapChain = InterfaceVariant<SwapChainBase,
#if VT_DYNAMIC_GRAPHICS_API
											  VT_GRAPHICS_API_NAME_PRIMARY::SwapChain,
#endif
											  VT_GRAPHICS_API_NAME::SwapChain>;
}
