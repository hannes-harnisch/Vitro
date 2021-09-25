module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.Buffer;

import vt.Graphics.Vulkan.Driver;

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
