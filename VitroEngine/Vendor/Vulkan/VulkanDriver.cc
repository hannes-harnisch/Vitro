module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"

#include <memory>
#include <unordered_map>
export module vt.Vulkan.Driver;

import vt.App.SharedLibrary;
import vt.Core.Singleton;
import vt.Graphics.DriverBase;

namespace vt::vulkan
{
	struct InstanceFunctionTable
	{
		VkInstance instance;

#define INSTANCE_FUNC(func) PFN_##func func = reinterpret_cast<PFN_##func>(vkGetInstanceProcAddr(instance, #func));

		INSTANCE_FUNC(vkCreateDebugUtilsMessengerEXT)
		INSTANCE_FUNC(vkCreateDevice)
#if VT_SYSTEM_MODULE == Windows
		INSTANCE_FUNC(vkCreateWin32SurfaceKHR)
#elif VT_SYSTEM_MODULE == Linux
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
		INSTANCE_FUNC(vkGetPhysicalDeviceProperties)
		INSTANCE_FUNC(vkGetPhysicalDeviceQueueFamilyProperties)
		INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
		INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceFormatsKHR)
		INSTANCE_FUNC(vkGetPhysicalDeviceSurfacePresentModesKHR)
		INSTANCE_FUNC(vkGetPhysicalDeviceSurfaceSupportKHR)

		InstanceFunctionTable(VkInstance instance) : instance(instance)
		{}
	};

	export class VulkanDriver final : public Singleton<VulkanDriver>, public DriverBase
	{
	public:
		VulkanDriver()
		{
			VkApplicationInfo const app_info {
				.sType		 = VK_STRUCTURE_TYPE_APPLICATION_INFO,
				.pEngineName = VT_ENGINE_NAME,
			};
			VkInstanceCreateInfo const instance_info {
				.sType			  = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pApplicationInfo = &app_info,
			};
			VkInstance raw_instance;

			auto result = vkCreateInstance(&instance_info, nullptr, &raw_instance);
			instance.reset(raw_instance);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan instance.");
		}

		DeviceFunctionTable const* get_owner(void* resource) const
		{
			return resource_owners.find(resource)->second;
		}

		template<typename T> T load_device_func(VkDevice device, char const name[]) const
		{
			return reinterpret_cast<T>(api->vkGetDeviceProcAddr(device, name));
		}

	private:
		SharedLibrary driver = "vulkan-1";

		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = driver.load_symbol<decltype(::vkGetInstanceProcAddr)>(
			"vkGetInstanceProcAddr");

#define INSTANCE_NULL_FUNC(func) PFN_##func func = reinterpret_cast<PFN_##func>(vkGetInstanceProcAddr(nullptr, #func));

		INSTANCE_NULL_FUNC(vkCreateInstance)
		INSTANCE_NULL_FUNC(vkEnumerateInstanceExtensionProperties)
		INSTANCE_NULL_FUNC(vkEnumerateInstanceLayerProperties)

		struct InstanceDeleter
		{
			using pointer = VkInstance;
			void operator()(VkInstance instance)
			{
				VulkanDriver::get().api->vkDestroyInstance(instance, nullptr);
			}
		};
		std::unique_ptr<VkInstance, InstanceDeleter>		  instance;
		std::unique_ptr<InstanceFunctionTable>				  api;
		std::unordered_map<void*, DeviceFunctionTable const*> resource_owners;
	};

	export struct DeviceFunctionTable
	{
		VkDevice device;

#define DEVICE_FUNC(func) PFN_##func func = VulkanDriver::get().load_device_func<PFN_##func>(device, #func);

		DEVICE_FUNC(vkAcquireNextImageKHR)
		DEVICE_FUNC(vkAllocateCommandBuffers)
		DEVICE_FUNC(vkAllocateDescriptorSets)
		DEVICE_FUNC(vkBeginCommandBuffer)
		DEVICE_FUNC(vkCmdBeginRenderPass)
		DEVICE_FUNC(vkCmdBindDescriptorSets)
		DEVICE_FUNC(vkCmdBindIndexBuffer)
		DEVICE_FUNC(vkCmdBindPipeline)
		DEVICE_FUNC(vkCmdBindVertexBuffers)
		DEVICE_FUNC(vkCmdDispatch)
		DEVICE_FUNC(vkCmdDraw)
		DEVICE_FUNC(vkCmdDrawIndexed)
		DEVICE_FUNC(vkCmdEndRenderPass)
		DEVICE_FUNC(vkCmdNextSubpass)
		DEVICE_FUNC(vkCmdPipelineBarrier)
		DEVICE_FUNC(vkCmdPushConstants)
		DEVICE_FUNC(vkCmdSetScissor)
		DEVICE_FUNC(vkCmdSetViewport)
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
		DEVICE_FUNC(vkDestroyRenderPass)
		DEVICE_FUNC(vkDestroySampler)
		DEVICE_FUNC(vkDestroySemaphore)
		DEVICE_FUNC(vkDestroyShaderModule)
		DEVICE_FUNC(vkDestroySwapchainKHR)
		DEVICE_FUNC(vkDeviceWaitIdle)
		DEVICE_FUNC(vkEndCommandBuffer)
		DEVICE_FUNC(vkGetSwapchainImagesKHR)
		DEVICE_FUNC(vkQueuePresentKHR)
		DEVICE_FUNC(vkQueueSubmit)
		DEVICE_FUNC(vkQueueWaitIdle)
		DEVICE_FUNC(vkResetCommandPool)
		DEVICE_FUNC(vkResetDescriptorPool)
		DEVICE_FUNC(vkUpdateDescriptorSets)
		DEVICE_FUNC(vkWaitForFences)
	};

	template<typename T>
	using DeviceTableFunctionPointerMember = void (*DeviceFunctionTable::*)(VkDevice, T, VkAllocationCallbacks const*);

	template<typename T, DeviceTableFunctionPointerMember<T> Deleter> struct DeviceResourceDeleter
	{
		using pointer = T;
		void operator()(T ptr)
		{
			auto table = VulkanDriver::get().get_owner(ptr);
			(table->*Deleter)(table->device, ptr, nullptr);
		}
	};
	template<typename T, DeviceTableFunctionPointerMember<T> Deleter>
	using UniqueVulkanResource = std::unique_ptr<T, DeviceResourceDeleter<T, Deleter>>;

#define EXPORT_UNIQUE_DEVICE_RESOURCE(resource)                                                                                \
	export using UniqueVk##resource = UniqueVulkanResource<Vk##resource, &DeviceFunctionTable::vkDestroy##resource>;

	EXPORT_UNIQUE_DEVICE_RESOURCE(Buffer)
	EXPORT_UNIQUE_DEVICE_RESOURCE(CommandPool)
	EXPORT_UNIQUE_DEVICE_RESOURCE(DescriptorPool)
	EXPORT_UNIQUE_DEVICE_RESOURCE(DescriptorSetLayout)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Fence)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Framebuffer)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Image)
	EXPORT_UNIQUE_DEVICE_RESOURCE(ImageView)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Pipeline)
	EXPORT_UNIQUE_DEVICE_RESOURCE(PipelineCache)
	EXPORT_UNIQUE_DEVICE_RESOURCE(PipelineLayout)
	EXPORT_UNIQUE_DEVICE_RESOURCE(RenderPass)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Sampler)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Semaphore)
	EXPORT_UNIQUE_DEVICE_RESOURCE(ShaderModule)
	EXPORT_UNIQUE_DEVICE_RESOURCE(SwapchainKHR)
}
