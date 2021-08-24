export module Vitro.Graphics.Device;

import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.Device;
#endif
import Vitro.VT_GPU_API_MODULE.Device;

namespace vt
{
	export using Device = InterfaceVariant<DeviceBase,
#if VT_DYNAMIC_GPU_API
										   VT_GPU_API_NAME_PRIMARY::Device,
#endif
										   VT_GPU_API_NAME::Device>;
}
