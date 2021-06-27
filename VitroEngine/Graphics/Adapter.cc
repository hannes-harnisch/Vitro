export module Vitro.Graphics.Adapter;

import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.Adapter;
import Vitro.Vulkan.Adapter;

export using Adapter = InterfaceObject<D3D12::Adapter, Vulkan::Adapter>;

#else

import Vitro.VT_GHI.Adapter;

export using Adapter = InterfaceObject<VT_GHI::Adapter>;

#endif
