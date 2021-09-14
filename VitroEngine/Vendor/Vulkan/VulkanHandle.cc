module;
#include "VulkanAPI.hh"
export module vt.Vulkan.Handle;

namespace vt::vulkan
{
	export using VulkanAdapter			 = VkPhysicalDevice;
	export using VulkanCommandListHandle = VkCommandBuffer;
}
