module;
#include "VulkanAPI.hpp"

#include <memory>
export module vt.Graphics.Vulkan.Handle;

import vt.Core.Scope;
import vt.Core.Singleton;

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

	// This struct is essentially a globally unique vtable for all Vulkan functions that are either device-independent, need to
	// be called before device creation or where any dispatch overhead inside the vulkan-1 shared library is acceptable. It also
	// stores the global VkInstance. Storing pointers to it is safe because it will exist in its own heap allocation.
	export struct InstanceApiTable : Singleton<InstanceApiTable>, NoCopyNoMove
	{
		PFN_vkGetInstanceProcAddr const vkGetInstanceProcAddr;
		VkInstance const				instance;

#define INSTANCE_FUNC(FUNC) PFN_##FUNC const FUNC = reinterpret_cast<PFN_##FUNC>(vkGetInstanceProcAddr(instance, #FUNC));

		INSTANCE_FUNC(vkCreateDebugUtilsMessengerEXT)
		INSTANCE_FUNC(vkCreateDevice)
#if VT_SYSTEM_WINDOWS
		INSTANCE_FUNC(vkCreateWin32SurfaceKHR)
#elif VT_SYSTEM_LINUX
		INSTANCE_FUNC(vkCreateXcbSurfaceKHR)
#endif
		INSTANCE_FUNC(vkDestroyDebugUtilsMessengerEXT)
		INSTANCE_FUNC(vkDestroyDevice)
		INSTANCE_FUNC(vkDestroyInstance)
		INSTANCE_FUNC(vkDestroySurfaceKHR)
		INSTANCE_FUNC(vkEnumerateDeviceExtensionProperties)
		INSTANCE_FUNC(vkEnumeratePhysicalDevices)
		INSTANCE_FUNC(vkGetDeviceProcAddr)
		INSTANCE_FUNC(vkGetPhysicalDeviceFeatures)
		INSTANCE_FUNC(vkGetPhysicalDeviceMemoryProperties)
		INSTANCE_FUNC(vkGetPhysicalDeviceProperties)
		INSTANCE_FUNC(vkGetPhysicalDeviceQueueFamilyProperties)
		INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
		INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceFormatsKHR)
		INSTANCE_FUNC(vkGetPhysicalDeviceSurfacePresentModesKHR)
		INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceSupportKHR)

		InstanceApiTable(PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr, VkInstance instance) :
			vkGetInstanceProcAddr(vkGetInstanceProcAddr), instance(instance)
		{}
	};

	// This struct is essentially a custom vtable for all the Vulkan functions associated with a specific Vulkan device as well
	// as providing the device, physical device and allocator itself. It loads the device-specific functions right from the
	// device driver. This way is recommended since using the global function prototypes and implicit dynamic linking requires
	// the Vulkan shared library to dispatch manually to the right device driver, which is entirely avoidable overhead. Keeping
	// pointers to it around is safe, since a specific instance of this table is only destroyed when the VkDevice is destroyed
	// and it exists in its own heap allocation.
	export struct DeviceApiTable : NoCopyNoMove
	{
		PFN_vkGetDeviceProcAddr const vkGetDeviceProcAddr;
		VkPhysicalDevice const		  adapter;
		VkDevice const				  device;
		VmaAllocator				  allocator;

#define DEVICE_FUNC(FUNC) PFN_##FUNC const FUNC = reinterpret_cast<PFN_##FUNC>(vkGetDeviceProcAddr(device, #FUNC));

		DEVICE_FUNC(vkAcquireNextImageKHR)
		DEVICE_FUNC(vkAllocateCommandBuffers)
		DEVICE_FUNC(vkAllocateDescriptorSets)
		DEVICE_FUNC(vkAllocateMemory)
		DEVICE_FUNC(vkBeginCommandBuffer)
		DEVICE_FUNC(vkBindBufferMemory)
		DEVICE_FUNC(vkBindImageMemory)
		DEVICE_FUNC(vkCmdBeginQuery)
		DEVICE_FUNC(vkCmdBeginRenderPass)
		DEVICE_FUNC(vkCmdBindDescriptorSets)
		DEVICE_FUNC(vkCmdBindIndexBuffer)
		DEVICE_FUNC(vkCmdBindPipeline)
		DEVICE_FUNC(vkCmdBindVertexBuffers)
		DEVICE_FUNC(vkCmdCopyBuffer)
		DEVICE_FUNC(vkCmdCopyBufferToImage)
		DEVICE_FUNC(vkCmdCopyImage)
		DEVICE_FUNC(vkCmdDispatch)
		DEVICE_FUNC(vkCmdDispatchIndirect)
		DEVICE_FUNC(vkCmdDraw)
		DEVICE_FUNC(vkCmdDrawIndexed)
		DEVICE_FUNC(vkCmdDrawIndexedIndirect)
		DEVICE_FUNC(vkCmdDrawIndirect)
		DEVICE_FUNC(vkCmdEndQuery)
		DEVICE_FUNC(vkCmdEndRenderPass)
		DEVICE_FUNC(vkCmdNextSubpass)
		DEVICE_FUNC(vkCmdPipelineBarrier)
		DEVICE_FUNC(vkCmdPushConstants)
		DEVICE_FUNC(vkCmdSetBlendConstants)
		DEVICE_FUNC(vkCmdSetDepthBounds)
		DEVICE_FUNC(vkCmdSetScissor)
		DEVICE_FUNC(vkCmdSetStencilReference)
		DEVICE_FUNC(vkCmdSetViewport)
		DEVICE_FUNC(vkCmdUpdateBuffer)
		DEVICE_FUNC(vkCreateBuffer)
		DEVICE_FUNC(vkCreateCommandPool)
		DEVICE_FUNC(vkCreateComputePipelines)
		DEVICE_FUNC(vkCreateDescriptorPool)
		DEVICE_FUNC(vkCreateDescriptorSetLayout)
		DEVICE_FUNC(vkCreateFence)
		DEVICE_FUNC(vkCreateFramebuffer)
		DEVICE_FUNC(vkCreateGraphicsPipelines)
		DEVICE_FUNC(vkCreateImage)
		DEVICE_FUNC(vkCreateImageView)
		DEVICE_FUNC(vkCreatePipelineCache)
		DEVICE_FUNC(vkCreatePipelineLayout)
		DEVICE_FUNC(vkCreateQueryPool)
		DEVICE_FUNC(vkCreateRenderPass)
		DEVICE_FUNC(vkCreateSampler)
		DEVICE_FUNC(vkCreateSemaphore)
		DEVICE_FUNC(vkCreateShaderModule)
		DEVICE_FUNC(vkCreateSwapchainKHR)
		DEVICE_FUNC(vkDestroyBuffer)
		DEVICE_FUNC(vkDestroyCommandPool)
		DEVICE_FUNC(vkDestroyDescriptorPool)
		DEVICE_FUNC(vkDestroyDescriptorSetLayout)
		DEVICE_FUNC(vkDestroyFence)
		DEVICE_FUNC(vkDestroyFramebuffer)
		DEVICE_FUNC(vkDestroyImage)
		DEVICE_FUNC(vkDestroyImageView)
		DEVICE_FUNC(vkDestroyPipeline)
		DEVICE_FUNC(vkDestroyPipelineCache)
		DEVICE_FUNC(vkDestroyPipelineLayout)
		DEVICE_FUNC(vkDestroyQueryPool)
		DEVICE_FUNC(vkDestroyRenderPass)
		DEVICE_FUNC(vkDestroySampler)
		DEVICE_FUNC(vkDestroySemaphore)
		DEVICE_FUNC(vkDestroyShaderModule)
		DEVICE_FUNC(vkDestroySwapchainKHR)
		DEVICE_FUNC(vkDeviceWaitIdle)
		DEVICE_FUNC(vkEndCommandBuffer)
		DEVICE_FUNC(vkFlushMappedMemoryRanges)
		DEVICE_FUNC(vkFreeMemory)
		DEVICE_FUNC(vkGetBufferMemoryRequirements)
		DEVICE_FUNC(vkGetDeviceQueue)
		DEVICE_FUNC(vkGetFenceStatus)
		DEVICE_FUNC(vkGetImageMemoryRequirements)
		DEVICE_FUNC(vkGetPipelineCacheData)
		DEVICE_FUNC(vkGetSwapchainImagesKHR)
		DEVICE_FUNC(vkInvalidateMappedMemoryRanges)
		DEVICE_FUNC(vkMapMemory)
		DEVICE_FUNC(vkQueuePresentKHR)
		DEVICE_FUNC(vkQueueSubmit)
		DEVICE_FUNC(vkQueueWaitIdle)
		DEVICE_FUNC(vkResetCommandPool)
		DEVICE_FUNC(vkResetDescriptorPool)
		DEVICE_FUNC(vkResetFences)
		DEVICE_FUNC(vkUnmapMemory)
		DEVICE_FUNC(vkUpdateDescriptorSets)
		DEVICE_FUNC(vkWaitForFences)

		DeviceApiTable(PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr, VkPhysicalDevice adapter, VkDevice device) :
			vkGetDeviceProcAddr(vkGetDeviceProcAddr), adapter(adapter), device(device)
		{}
	};

	// Supply a Vulkan handle type H and you get a member object pointer type usable on DeviceApiTable that has the right type
	// to be called as the destroying function for H. This only makes sense to be used for the many Vulkan handles whose
	// destroying functions have the prototype "void (*)(VkDevice, H, VkAllocationCallbacks const*)".
	template<typename H>
	using DeviceTableFunctionPointerMember = void (*const DeviceApiTable::*)(VkDevice, H, VkAllocationCallbacks const*);

	// Can be used to create an appropriate custom deleter for std::unique_ptr that stores a pointer to the DeviceApiTable whose
	// device owns the given Vulkan handle H and will destroy the handle appropriately.
	template<typename H, DeviceTableFunctionPointerMember<H> DELETER> struct DeviceResourceDeleter
	{
		using pointer = H;

		DeviceApiTable const* owner;

		// Needs to be default-constructible otherwise unique pointers using it won't be.
		DeviceResourceDeleter() = default;

		DeviceResourceDeleter(DeviceApiTable const& owner) : owner(&owner)
		{}

		void operator()(H handle) const
		{
			(owner->*DELETER)(owner->device, handle, nullptr);
		}
	};

	// Reusable custom version of std::unique_ptr for Vulkan handles that are destroyed in a similar way. Makes RAII for Vulkan
	// way easier overall.
	template<typename T, DeviceTableFunctionPointerMember<T> DELETER>
	class UniqueDeviceResource : public std::unique_ptr<T, DeviceResourceDeleter<T, DELETER>>
	{
		using Base = std::unique_ptr<T, DeviceResourceDeleter<T, DELETER>>;

	public:
		using Base::Base;

		void reset(T resource, DeviceApiTable const& owner) noexcept
		{
			Base::reset(resource);
			this->get_deleter().owner = &owner;
		}
	};

	// Exports an alias for a unique pointer for a specific Vulkan handle.
#define EXPORT_UNIQUE_DEVICE_RESOURCE(RESOURCE)                                                                                \
	export using UniqueVk##RESOURCE = UniqueDeviceResource<Vk##RESOURCE, &DeviceApiTable::vkDestroy##RESOURCE>;

	EXPORT_UNIQUE_DEVICE_RESOURCE(CommandPool)
	EXPORT_UNIQUE_DEVICE_RESOURCE(DescriptorPool)
	EXPORT_UNIQUE_DEVICE_RESOURCE(DescriptorSetLayout)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Fence)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Framebuffer)
	EXPORT_UNIQUE_DEVICE_RESOURCE(ImageView)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Pipeline)
	EXPORT_UNIQUE_DEVICE_RESOURCE(PipelineCache)
	EXPORT_UNIQUE_DEVICE_RESOURCE(PipelineLayout)
	EXPORT_UNIQUE_DEVICE_RESOURCE(QueryPool)
	EXPORT_UNIQUE_DEVICE_RESOURCE(RenderPass)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Sampler)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Semaphore)
	EXPORT_UNIQUE_DEVICE_RESOURCE(ShaderModule)
	EXPORT_UNIQUE_DEVICE_RESOURCE(SwapchainKHR)
}
