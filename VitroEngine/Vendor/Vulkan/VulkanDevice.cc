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
		VulkanDevice()
		{}

		DeviceFunctionTable const* api() const
		{
			return function_table.get();
		}

	private:
		std::unique_ptr<DeviceFunctionTable> function_table;
	};
}
