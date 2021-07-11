export module Vitro.Graphics.Device;

import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GRAPHICS_API
import Vitro.VT_GRAPHICS_API_MODULE_PRIMARY.Device;
#endif
import Vitro.VT_GRAPHICS_API_MODULE.Device;

namespace vt
{
	export using Device = InterfaceVariant<DeviceBase,
#if VT_DYNAMIC_GRAPHICS_API
										   VT_GRAPHICS_API_NAME_PRIMARY::Device,
#endif
										   VT_GRAPHICS_API_NAME::Device>;
}
