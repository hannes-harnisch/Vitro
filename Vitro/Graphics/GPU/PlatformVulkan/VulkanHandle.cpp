module;
#include <vulkan/vulkan.h>
export module vt.Graphics.Vulkan.Handle;

namespace vt::vulkan
{
	export using VulkanAdapter			 = VkPhysicalDevice;
	export using VulkanCommandListHandle = VkCommandBuffer;

	export struct VulkanSyncValue
	{
		VkFence		fence	  = nullptr;
		VkSemaphore semaphore = nullptr;
	};
}
