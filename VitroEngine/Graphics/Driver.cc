export module Vitro.Graphics.Driver;

import Vitro.Graphics.DriverBase;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.Driver;
import Vitro.Vulkan.Driver;

namespace vt
{
	export using Driver = InterfaceVariant<DriverBase, D3D12::Driver, Vulkan::Driver>;
}

#else

import Vitro.VT_GRAPHICS_API_MODULE.Driver;

namespace vt
{
	export using Driver = InterfaceVariant<DriverBase, VT_GRAPHICS_API_NAME::Driver>;
}

#endif
