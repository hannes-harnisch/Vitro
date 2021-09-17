export module vt.Vulkan.DeviceChild;

import vt.Graphics.Device;
import vt.Vulkan.Driver;

namespace vt::vulkan
{
	export class DeviceChild
	{
	protected:
		DeviceFunctionTable const* api;

		DeviceChild(Device const& device) : api(device.vulkan.get_function_table())
		{}
	};
}
