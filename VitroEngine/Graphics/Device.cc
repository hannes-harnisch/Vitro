export module Vitro.Graphics.Device;

import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.Device;
import Vitro.Vulkan.Device;

namespace vt
{
	export using Device = InterfaceObject<DeviceBase, D3D12::Device, Vulkan::Device>;
}

#else

import Vitro.VT_GRAPHICS_API_MODULE.Device;

namespace vt
{
	export using Device = InterfaceObject<DeviceBase, VT_GRAPHICS_API_NAME::Device>;
}

#endif
