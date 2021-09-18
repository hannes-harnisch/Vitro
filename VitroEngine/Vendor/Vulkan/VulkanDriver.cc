module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"

#include <memory>
#include <ranges>
#include <string_view>
#include <unordered_map>
#include <vector>
export module vt.Vulkan.Driver;

import vt.App.SharedLibrary;
import vt.Core.Algorithm;
import vt.Core.Singleton;
import vt.Core.Version;
import vt.Graphics.DriverBase;
import vt.Graphics.Handle;
import vt.Trace.Log;

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
		INSTANCE_FUNC(vkGetPhysicalDeviceMemoryProperties)
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
		static InstanceFunctionTable const* get_api()
		{
			return get().api.get();
		}

		VulkanDriver(std::string const& app_name, Version app_version, Version engine_version)
		{
			ensure_instance_extensions_exist();
			ensure_layers_exist();

			VkDebugUtilsMessengerCreateInfoEXT const messenger_info {
				.sType			 = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
								   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
				.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
				.pfnUserCallback = log_vulkan_validation,
			};
			auto const validation_feature = VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT;

			VkValidationFeaturesEXT const validation_features {
				.sType						   = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
				.pNext						   = &messenger_info,
				.enabledValidationFeatureCount = 1,
				.pEnabledValidationFeatures	   = &validation_feature,
			};
#if VT_DEBUG
			void const* instance_info_next = &validation_features;
#else
			void const* instance_info_next = nullptr;
#endif
			VkApplicationInfo const app_info {
				.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO,
				.pApplicationName	= app_name.data(),
				.applicationVersion = VK_MAKE_VERSION(app_version.major, app_version.minor, app_version.patch),
				.pEngineName		= VT_ENGINE_NAME,
				.engineVersion		= VK_MAKE_VERSION(engine_version.major, engine_version.minor, engine_version.patch),
				.apiVersion			= VK_API_VERSION_1_0,
			};
			VkInstanceCreateInfo const instance_info {
				.sType					 = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pNext					 = instance_info_next,
				.pApplicationInfo		 = &app_info,
				.enabledLayerCount		 = RequiredInstanceLayer != nullptr,
				.ppEnabledLayerNames	 = &RequiredInstanceLayer,
				.enabledExtensionCount	 = count(RequiredInstanceExtensions),
				.ppEnabledExtensionNames = RequiredInstanceExtensions,
			};
			VkInstance raw_instance;

			auto result = vkCreateInstance(&instance_info, nullptr, &raw_instance);
			instance.reset(raw_instance);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan instance.");

			api = std::make_unique<InstanceFunctionTable>(instance.get());
#if VT_DEBUG
			VkDebugUtilsMessengerEXT raw_messenger;
			result = api->vkCreateDebugUtilsMessengerEXT(instance.get(), &messenger_info, nullptr, &raw_messenger);
			debug_messenger.reset(raw_messenger);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan debug utils messenger.");
#endif
		}

		std::vector<Adapter> enumerate_adapters() const override
		{
			unsigned count;
			auto	 result = api->vkEnumeratePhysicalDevices(instance.get(), &count, nullptr);
			VT_ENSURE_RESULT(result, "Failed to query Vulkan physical device count.");

			std::vector<VkPhysicalDevice> physical_devices(count);
			result = api->vkEnumeratePhysicalDevices(instance.get(), &count, physical_devices.data());
			VT_ENSURE_RESULT(result, "Failed to enumerate Vulkan instance extensions.");

			std::vector<Adapter> adapters;
			for(auto physical_device : physical_devices)
			{
				VkPhysicalDeviceProperties device_info;
				api->vkGetPhysicalDeviceProperties(physical_device, &device_info);
				if(device_info.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU &&
				   device_info.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
					continue;

				VkPhysicalDeviceMemoryProperties mem_info;
				api->vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_info);

				size_t vram = 0;
				for(auto heap : std::views::take(mem_info.memoryHeaps, mem_info.memoryHeapCount))
					vram += heap.size;

				adapters.emplace_back(physical_device, device_info.deviceName, vram);
			}
			return adapters;
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
		static constexpr char const* RequiredInstanceLayer =
#if VT_DEBUG
			"VK_LAYER_KHRONOS_validation";
#else
			{};
#endif

		static constexpr char const* RequiredInstanceExtensions[]
		{
			VK_KHR_SURFACE_EXTENSION_NAME,
#if VT_DEBUG
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
#if VT_SYSTEM_MODULE == Windows
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif VT_SYSTEM_MODULE == Linux
				VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
		};

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
				VulkanDriver::get_api()->vkDestroyInstance(instance, nullptr);
			}
		};
		std::unique_ptr<VkInstance, InstanceDeleter> instance;
		std::unique_ptr<InstanceFunctionTable const> api;

		struct DebugUtilsMessengerDeleter
		{
			using pointer = VkDebugUtilsMessengerEXT;
			void operator()(VkDebugUtilsMessengerEXT messenger)
			{
				auto api = VulkanDriver::get_api();
				api->vkDestroyDebugUtilsMessengerEXT(api->instance, messenger, nullptr);
			}
		};
		std::unique_ptr<VkDebugUtilsMessengerEXT, DebugUtilsMessengerDeleter> debug_messenger;
		std::unordered_map<void*, DeviceFunctionTable const*>				  resource_owners;

		static VKAPI_ATTR VkBool32 VKAPI_CALL log_vulkan_validation(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
																	VkDebugUtilsMessageTypeFlagsEXT,
																	VkDebugUtilsMessengerCallbackDataEXT const* callback_data,
																	void*)
		{
			switch(severity)
			{
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Log().verbose(callback_data->pMessage); break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Log().info(callback_data->pMessage); break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Log().warn(callback_data->pMessage); break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Log().error(callback_data->pMessage); break;
			}
			return false;
		}

		void ensure_instance_extensions_exist()
		{
			unsigned count;
			auto	 result = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
			VT_ENSURE_RESULT(result, "Failed to query Vulkan instance extension count.");

			std::vector<VkExtensionProperties> extensions(count);
			result = vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
			VT_ENSURE_RESULT(result, "Failed to enumerate Vulkan instance extensions.");

			for(auto requiredExtension : RequiredInstanceExtensions)
			{
				bool found = false;
				for(auto& extension : extensions)
				{
					if(extension.extensionName == std::string_view(requiredExtension))
					{
						found = true;
						break;
					}
				}
				VT_ENSURE(found, "Failed to find required Vulkan instance extension.");
			}
		}

		void ensure_layers_exist()
		{
			unsigned count;
			auto	 result = vkEnumerateInstanceLayerProperties(&count, nullptr);
			VT_ENSURE_RESULT(result, "Failed to query Vulkan instance layer count.");

			std::vector<VkLayerProperties> layers(count);
			result = vkEnumerateInstanceLayerProperties(&count, layers.data());
			VT_ENSURE_RESULT(result, "Failed to enumerate Vulkan layers.");

			bool found = false;
			for(auto& layer : layers)
			{
				if(layer.layerName == std::string_view(RequiredInstanceLayer))
				{
					found = true;
					break;
				}
			}
			VT_ENSURE(found, "Failed to find required Vulkan layer.");
		}
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
