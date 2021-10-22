module;
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.Handle;

namespace vt::vulkan
{
	export using VulkanAdapter			 = VkPhysicalDevice;
	export using VulkanCommandListHandle = VkCommandBuffer;

	export struct VulkanSyncToken
	{
		VkFence		fence	  = nullptr;
		VkSemaphore semaphore = nullptr;
		uint64_t	resets	  = 0;
	};
}
