export module Vitro.Graphics.Driver;

import Vitro.Graphics.DriverBase;
import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GHI

import Vitro.D3D12.Driver;
import Vitro.Vulkan.Driver;

namespace vt
{
	export using Driver = InterfaceObject<DriverBase, D3D12::Driver, Vulkan::Driver>;
}

#else

import Vitro.VT_GHI_MODULE.Driver;

namespace vt
{
	export using Driver = InterfaceObject<DriverBase, VT_GHI_NAME::Driver>;
}

#endif
