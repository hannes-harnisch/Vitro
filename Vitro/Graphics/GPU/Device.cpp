module;
#include "Core/Macros.hpp"
export module vt.Graphics.Device;

import vt.Graphics.DeviceBase;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.Handle;
import vt.Graphics.VT_GPU_API_MODULE.Device;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.Device;
#endif

namespace vt
{
	using PlatformDevice = InterfaceVariant<DeviceBase, VT_GPU_API_VARIANT_ARGS(Device)>;
	export class Device : public PlatformDevice
	{
	public:
		using PlatformDevice::PlatformDevice;
	};
}
