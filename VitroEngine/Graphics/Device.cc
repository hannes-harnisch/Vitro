export module Vitro.Graphics.Device;

import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.Device;
import Vitro.Vulkan.Device;

export using Device = InterfaceObject<D3D12::Device, Vulkan::Device>;

#else

import Vitro.VT_GHI.Device;

export using Device = InterfaceObject<VT_GHI::Device>;

#endif
