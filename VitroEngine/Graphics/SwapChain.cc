export module Vitro.Graphics.SwapChain;

import Vitro.Graphics.InterfaceVariant;
import Vitro.Graphics.SwapChainBase;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.SwapChain;
import Vitro.Vulkan.SwapChain;

namespace vt
{
	export using SwapChain = InterfaceVariant<SwapChainBase, D3D12::SwapChain, Vulkan::SwapChain>;
}

#else

import Vitro.VT_GRAPHICS_API_MODULE.SwapChain;

namespace vt
{
	export using SwapChain = InterfaceVariant<SwapChainBase, VT_GRAPHICS_API_NAME::SwapChain>;
}

#endif
