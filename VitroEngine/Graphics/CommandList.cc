export module Vitro.Graphics.CommandList;

import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.CommandList;
import Vitro.Vulkan.CommandList;

export using SwapChain = InterfaceObject<D3D12::CommandList, Vulkan::CommandList>;

#else

import Vitro.VT_GHI.CommandList;

export using CommandList = InterfaceObject<VT_GHI::CommandList>;

#endif
