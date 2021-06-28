export module Vitro.Graphics.SwapChain;

import Vitro.Graphics.InterfaceObject;
import Vitro.Graphics.SwapChainBase;

#if VT_DYNAMIC_GHI

import Vitro.D3D12.SwapChain;
import Vitro.Vulkan.SwapChain;

export using SwapChain = InterfaceObject<SwapChainBase, D3D12::SwapChain, Vulkan::SwapChain>;

#else

import Vitro.VT_GHI.SwapChain;

export using SwapChain = InterfaceObject<SwapChainBase, VT_GHI::SwapChain>;

#endif
