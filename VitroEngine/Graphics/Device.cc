export module Vitro.Graphics.Device;

import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.Device;
import Vitro.Vulkan.Device;

namespace vt
{
	export using Device = InterfaceVariant<DeviceBase, D3D12::Device, Vulkan::Device>;
}

#else

import Vitro.VT_GRAPHICS_API_MODULE.Device;

namespace vt
{
	export using Device = InterfaceVariant<DeviceBase, VT_GRAPHICS_API_NAME::Device>;
}

#endif
