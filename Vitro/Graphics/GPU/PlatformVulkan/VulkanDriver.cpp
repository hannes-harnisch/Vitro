module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
#include <ranges>
#include <string_view>
#include <unordered_map>
#include <vector>
export module vt.Graphics.Vulkan.Driver;

import vt.Core.Array;
import vt.Core.Scope;
import vt.Core.SharedLibrary;
import vt.Core.Singleton;
import vt.Core.Version;
import vt.Graphics.DriverBase;
import vt.Graphics.Handle;
import vt.Trace.Log;

namespace vt::vulkan
{
	export struct InstanceApiTable : NoCopyNoMove
	{
		VkInstance instance;

#define INSTANCE_FUNC(FUNC) PFN_##FUNC FUNC = reinterpret_cast<PFN_##FUNC>(vkGetInstanceProcAddr(instance, #FUNC));

		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
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

		InstanceApiTable(VkInstance instance, PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr) :
			instance(instance), vkGetInstanceProcAddr(vkGetInstanceProcAddr)
		{}
	};

	export class VulkanDriver final : public Singleton<VulkanDriver>, public DriverBase
	{
	public:
		// Returns a pointer (never null) to the device-independent table of Vulkan functions.
		static InstanceApiTable const* get_api()
		{
			return get().api.get();
		}

		VulkanDriver(bool enable_debug_layer, std::string const& app_name, Version app_version, Version engine_version) :
			driver_dylib("vulkan-1")
		{
			ensure_instance_extensions_exist();
			if(enable_debug_layer)
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
			VkValidationFeaturesEXT const validation_features {
				.sType						   = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
				.pNext						   = &messenger_info,
				.enabledValidationFeatureCount = count(VALIDATION_FEATURES),
				.pEnabledValidationFeatures	   = VALIDATION_FEATURES,
			};
			VkApplicationInfo const app_info {
				.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO,
				.pApplicationName	= app_name.data(),
				.applicationVersion = VK_MAKE_VERSION(app_version.major, app_version.minor, app_version.patch),
				.pEngineName		= VT_ENGINE_NAME,
				.engineVersion		= VK_MAKE_VERSION(engine_version.major, engine_version.minor, engine_version.patch),
				.apiVersion			= VK_API_VERSION_1_0,
			};

			// Debug layer is last in list. Include only if debug is specified.
			uint32_t extension_count = count(REQUIRED_INSTANCE_EXTENSIONS) - !enable_debug_layer;

			VkInstanceCreateInfo const instance_info {
				.sType					 = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pNext					 = enable_debug_layer ? &validation_features : nullptr,
				.pApplicationInfo		 = &app_info,
				.enabledLayerCount		 = enable_debug_layer ? count(LAYERS) : 0,
				.ppEnabledLayerNames	 = enable_debug_layer ? LAYERS : nullptr,
				.enabledExtensionCount	 = extension_count,
				.ppEnabledExtensionNames = REQUIRED_INSTANCE_EXTENSIONS,
			};
			auto result = vkCreateInstance(&instance_info, nullptr, std::out_ptr(instance));
			api			= std::make_unique<InstanceApiTable>(instance.get(), vkGetInstanceProcAddr);
			VT_CHECK_RESULT(result, "Failed to create Vulkan instance.");

			if(enable_debug_layer)
			{
				result = api->vkCreateDebugUtilsMessengerEXT(instance.get(), &messenger_info, nullptr, std::out_ptr(messenger));
				VT_CHECK_RESULT(result, "Failed to create Vulkan debug utils messenger.");
			}
		}

		SmallList<Adapter> enumerate_adapters() const override
		{
			uint32_t device_count;
			auto	 result = api->vkEnumeratePhysicalDevices(instance.get(), &device_count, nullptr);
			VT_CHECK_RESULT(result, "Failed to query Vulkan physical device count.");

			SmallList<VkPhysicalDevice> physical_devices(device_count);
			result = api->vkEnumeratePhysicalDevices(instance.get(), &device_count, physical_devices.data());
			VT_CHECK_RESULT(result, "Failed to enumerate Vulkan instance extensions.");

			SmallList<Adapter> adapters;
			for(auto physical_device : physical_devices)
			{
				VkPhysicalDeviceProperties device_prop;
				api->vkGetPhysicalDeviceProperties(physical_device, &device_prop);
				if(device_prop.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU &&
				   device_prop.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
					continue;

				VkPhysicalDeviceMemoryProperties mem_prop;
				api->vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_prop);

				size_t vram = 0;
				for(auto heap : std::views::take(mem_prop.memoryHeaps, mem_prop.memoryHeapCount))
					vram += heap.size;

				adapters.emplace_back(physical_device, device_prop.deviceName, vram);
			}
			return adapters;
		}

		template<typename F> F load_device_func(VkDevice device, char const name[]) const
		{
			return reinterpret_cast<F>(api->vkGetDeviceProcAddr(device, name));
		}

		void notify_new_resource(void* resource, struct DeviceApiTable const& owner)
		{
			resource_owners.try_emplace(resource, &owner);
		}

		DeviceApiTable const* get_owner_and_erase(void* resource)
		{
			auto it	   = resource_owners.find(resource);
			auto owner = it->second;
			resource_owners.erase(it);
			return owner;
		}

	private:
		static constexpr char const* LAYERS[] = {
			"VK_LAYER_KHRONOS_validation",
		};

		static constexpr VkValidationFeatureEnableEXT VALIDATION_FEATURES[] = {
			VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
			VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
		};

		static constexpr char const* REQUIRED_INSTANCE_EXTENSIONS[] = {
#if VT_SYSTEM_WINDOWS
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif VT_SYSTEM_LINUX
			VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif

			VK_KHR_SURFACE_EXTENSION_NAME,

			// Will not actually get submitted unless debug features are requested during driver creation. This should always be
			// the last extension.
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		};

		struct InstanceDeleter
		{
			using pointer = VkInstance;
			void operator()(VkInstance instance) const
			{
				VulkanDriver::get_api()->vkDestroyInstance(instance, nullptr);
			}
		};
		using UniqueVkInstance = std::unique_ptr<VkInstance, InstanceDeleter>;

		struct MessengerDeleter
		{
			using pointer = VkDebugUtilsMessengerEXT;
			void operator()(VkDebugUtilsMessengerEXT messenger) const
			{
				auto driver = VulkanDriver::get_api();
				driver->vkDestroyDebugUtilsMessengerEXT(driver->instance, messenger, nullptr);
			}
		};
		using UniqueVkDebugUtilsMessengerEXT = std::unique_ptr<VkDebugUtilsMessengerEXT, MessengerDeleter>;

#define INSTANCE_NULL_FUNC(FUNC) PFN_##FUNC FUNC = reinterpret_cast<PFN_##FUNC>(vkGetInstanceProcAddr(nullptr, #FUNC));

		SharedLibrary			  driver_dylib;
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = driver_dylib.load_symbol<decltype(::vkGetInstanceProcAddr)>(
			"vkGetInstanceProcAddr");
		std::unique_ptr<InstanceApiTable const> api;
		INSTANCE_NULL_FUNC(vkCreateInstance)
		INSTANCE_NULL_FUNC(vkEnumerateInstanceExtensionProperties)
		INSTANCE_NULL_FUNC(vkEnumerateInstanceLayerProperties)
		UniqueVkInstance								 instance;
		UniqueVkDebugUtilsMessengerEXT					 messenger;
		std::unordered_map<void*, DeviceApiTable const*> resource_owners;

		static VKAPI_ATTR VkBool32 VKAPI_CALL log_vulkan_validation(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
																	VkDebugUtilsMessageTypeFlagsEXT,
																	VkDebugUtilsMessengerCallbackDataEXT const* callback_data,
																	void*)
		{
			auto msg = callback_data->pMessage;
			switch(severity)
			{
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Log().info(msg, "\n"); break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Log().warn(msg, "\n"); break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Log().error(msg, "\n"); break;
			}
			return false;
		}

		void ensure_instance_extensions_exist() const
		{
			uint32_t extension_count;
			auto	 result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
			VT_CHECK_RESULT(result, "Failed to query Vulkan instance extension count.");

			std::vector<VkExtensionProperties> extensions(extension_count);
			result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
			VT_CHECK_RESULT(result, "Failed to enumerate Vulkan instance extensions.");

			for(std::string_view required_ext : REQUIRED_INSTANCE_EXTENSIONS)
			{
				bool found = false;
				for(auto& extension : extensions)
				{
					if(extension.extensionName == required_ext)
					{
						found = true;
						break;
					}
				}
				VT_ENSURE(found, "Failed to find required Vulkan instance extension.");
			}
		}

		void ensure_layers_exist() const
		{
			uint32_t layer_count;
			auto	 result = vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
			VT_CHECK_RESULT(result, "Failed to query Vulkan instance layer count.");

			std::vector<VkLayerProperties> layers(layer_count);
			result = vkEnumerateInstanceLayerProperties(&layer_count, layers.data());
			VT_CHECK_RESULT(result, "Failed to enumerate Vulkan layers.");

			for(std::string_view required_layer : LAYERS)
			{
				bool found = false;
				for(auto& layer : layers)
				{
					if(layer.layerName == required_layer)
					{
						found = true;
						break;
					}
				}
				VT_ENSURE(found, "Failed to find required Vulkan layer.");
			}
		}
	};

	export struct DeviceApiTable : NoCopyNoMove
	{
		VkPhysicalDevice adapter;
		VkDevice		 device;

#define DEVICE_FUNC(FUNC) PFN_##FUNC FUNC = VulkanDriver::get().load_device_func<PFN_##FUNC>(device, #FUNC);

		DEVICE_FUNC(vkAcquireNextImageKHR)
		DEVICE_FUNC(vkAllocateCommandBuffers)
		DEVICE_FUNC(vkAllocateDescriptorSets)
		DEVICE_FUNC(vkAllocateMemory)
		DEVICE_FUNC(vkBeginCommandBuffer)
		DEVICE_FUNC(vkBindBufferMemory)
		DEVICE_FUNC(vkBindImageMemory)
		DEVICE_FUNC(vkCmdBeginRenderPass)
		DEVICE_FUNC(vkCmdBindDescriptorSets)
		DEVICE_FUNC(vkCmdBindIndexBuffer)
		DEVICE_FUNC(vkCmdBindPipeline)
		DEVICE_FUNC(vkCmdBindVertexBuffers)
		DEVICE_FUNC(vkCmdCopyBuffer)
		DEVICE_FUNC(vkCmdDispatch)
		DEVICE_FUNC(vkCmdDispatchIndirect)
		DEVICE_FUNC(vkCmdDraw)
		DEVICE_FUNC(vkCmdDrawIndexed)
		DEVICE_FUNC(vkCmdDrawIndexedIndirect)
		DEVICE_FUNC(vkCmdDrawIndirect)
		DEVICE_FUNC(vkCmdEndRenderPass)
		DEVICE_FUNC(vkCmdNextSubpass)
		DEVICE_FUNC(vkCmdPipelineBarrier)
		DEVICE_FUNC(vkCmdPushConstants)
		DEVICE_FUNC(vkCmdSetBlendConstants)
		DEVICE_FUNC(vkCmdSetDepthBounds)
		DEVICE_FUNC(vkCmdSetScissor)
		DEVICE_FUNC(vkCmdSetStencilReference)
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

		DeviceApiTable(VkPhysicalDevice adapter, VkDevice device) : adapter(adapter), device(device)
		{}
	};

	template<typename T>
	using DeviceTableFunctionPointerMember = void (*DeviceApiTable::*)(VkDevice, T, VkAllocationCallbacks const*);

	template<typename T, DeviceTableFunctionPointerMember<T> DELETER> struct DeviceResourceDeleter
	{
		using pointer = T;
		void operator()(T ptr) const
		{
			auto table = VulkanDriver::get().get_owner_and_erase(ptr);
			(table->*DELETER)(table->device, ptr, nullptr);
		}
	};

	template<typename T, DeviceTableFunctionPointerMember<T> DELETER>
	class UniqueDeviceResource : public std::unique_ptr<T, DeviceResourceDeleter<T, DELETER>>
	{
		using Base = std::unique_ptr<T, DeviceResourceDeleter<T, DELETER>>;

	public:
		UniqueDeviceResource() = default;

		UniqueDeviceResource(T resource, DeviceApiTable const& owner) : Base(resource)
		{
			VulkanDriver::get().notify_new_resource(resource, owner);
		}

		void reset(T resource, DeviceApiTable const& owner) noexcept
		{
			Base::reset(resource);
			VulkanDriver::get().notify_new_resource(resource, owner);
		}
	};

#define EXPORT_UNIQUE_DEVICE_RESOURCE(RESOURCE)                                                                                \
	export using UniqueVk##RESOURCE = UniqueDeviceResource<Vk##RESOURCE, &DeviceApiTable::vkDestroy##RESOURCE>;

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
	EXPORT_UNIQUE_DEVICE_RESOURCE(QueryPool)
	EXPORT_UNIQUE_DEVICE_RESOURCE(RenderPass)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Sampler)
	EXPORT_UNIQUE_DEVICE_RESOURCE(Semaphore)
	EXPORT_UNIQUE_DEVICE_RESOURCE(ShaderModule)
	EXPORT_UNIQUE_DEVICE_RESOURCE(SwapchainKHR)
}
