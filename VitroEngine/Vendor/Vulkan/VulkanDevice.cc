module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"

#include <memory>
export module vt.Vulkan.Device;

import vt.Graphics.DeviceBase;
import vt.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanDevice final : public DeviceBase
	{
	public:
		DeviceFunctionTable const* get_function_table() const
		{
			return api.get();
		}

	private:
		std::unique_ptr<DeviceFunctionTable> api;
	};
}
