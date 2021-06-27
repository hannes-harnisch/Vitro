export module Vitro.Graphics.SwapChain;

import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.SwapChain;
import Vitro.Vulkan.SwapChain;

export using SwapChain = InterfaceObject<D3D12::SwapChain, Vulkan::SwapChain>;

#else

import Vitro.VT_GHI.SwapChain;

export using SwapChain = InterfaceObject<VT_GHI::SwapChain>;

#endif
