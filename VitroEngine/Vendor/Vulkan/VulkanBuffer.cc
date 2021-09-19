module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"
export module vt.Vulkan.Buffer;

import vt.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanBuffer
	{
	public:
		VulkanBuffer()
		{}

		VkBuffer ptr() const
		{
			return buffer.get();
		}

	private:
		UniqueVkBuffer buffer;
	};
}
