module;
#include "Core/Macros.hh"
export module vt.Graphics.Device;

import vt.Graphics.DeviceBase;
import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.Device;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.Device;
#endif

namespace vt
{
	export using Device = InterfaceVariant<DeviceBase, VT_GPU_API_VARIANT_ARGS(Device)>;
}
