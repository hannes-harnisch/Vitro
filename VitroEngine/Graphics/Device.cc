export module Vitro.Graphics.Device;

import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GHI

import Vitro.D3D12.Device;
import Vitro.Vulkan.Device;

export using Device = InterfaceObject<DeviceBase, D3D12::Device, Vulkan::Device>;

#else

import Vitro.VT_GHI.Device;

export using Device = InterfaceObject<DeviceBase, VT_GHI::Device>;

#endif
