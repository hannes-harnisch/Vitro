﻿module;
#include "Core/Macros.hh"
export module vt.Graphics.Device;

import vt.Graphics.DeviceBase;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.Handle;
import vt.VT_GPU_API_MODULE.Device;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.Device;
#endif

namespace vt
{
	using PlatformDevice = InterfaceVariant<DeviceBase, VT_GPU_API_VARIANT_ARGS(Device)>;
	export class Device : public PlatformDevice
	{
	public:
		Device(Adapter adapter) : PlatformDevice(std::move(adapter))
		{}
	};
}
