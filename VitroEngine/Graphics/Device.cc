export module vt.Graphics.Device;

import vt.Graphics.DeviceBase;
import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.Device;
#endif
import vt.VT_GPU_API_MODULE.Device;

namespace vt
{
	export using Device = InterfaceVariant<DeviceBase,
#if VT_DYNAMIC_GPU_API
										   VT_GPU_API_NAME_PRIMARY::Device,
#endif
										   VT_GPU_API_NAME::Device>;
}
