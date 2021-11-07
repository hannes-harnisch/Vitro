module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
#include <ranges>
#include <string_view>
#include <vector>
export module vt.Graphics.Vulkan.Driver;

import vt.Core.Array;
import vt.Core.SharedLibrary;
import vt.Core.Version;
import vt.Graphics.DriverBase;
import vt.Graphics.Handle;
import vt.Trace.Log;

namespace vt::vulkan
{
	export class VulkanDriver final : public DriverBase
	{
	public:
		VulkanDriver(bool enable_debug_layer, std::string const& app_name, Version app_version, Version engine_version) :
			loader("vulkan-1"),
			vkGetInstanceProcAddr(loader.load_symbol<std::remove_pointer_t<PFN_vkGetInstanceProcAddr>>("vkGetInstanceProcAddr"))
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
				.pUserData		 = nullptr,
			};
			VkValidationFeaturesEXT const validation_features {
				.sType							= VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
				.pNext							= &messenger_info,
				.enabledValidationFeatureCount	= count(VALIDATION_FEATURES),
				.pEnabledValidationFeatures		= VALIDATION_FEATURES,
				.disabledValidationFeatureCount = 0,
				.pDisabledValidationFeatures	= nullptr,
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

			api = std::make_unique<InstanceApiTable>(vkGetInstanceProcAddr, instance.get());
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

				adapters.emplace_back(VulkanAdapter(physical_device), device_prop.deviceName, vram);
			}
			return adapters;
		}

		Device make_device(Adapter const& adapter) const override
		{
			return VulkanDevice(adapter);
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
				InstanceApiTable::get().vkDestroyInstance(instance, nullptr);
			}
		};
		using UniqueVkInstance = std::unique_ptr<VkInstance, InstanceDeleter>;

		struct MessengerDeleter
		{
			using pointer = VkDebugUtilsMessengerEXT;
			void operator()(VkDebugUtilsMessengerEXT messenger) const
			{
				auto& driver = InstanceApiTable::get();
				driver.vkDestroyDebugUtilsMessengerEXT(driver.instance, messenger, nullptr);
			}
		};
		using UniqueVkDebugUtilsMessengerEXT = std::unique_ptr<VkDebugUtilsMessengerEXT, MessengerDeleter>;

#define INSTANCE_NULL_FUNC(FUNC) PFN_##FUNC FUNC = reinterpret_cast<PFN_##FUNC>(vkGetInstanceProcAddr(nullptr, #FUNC));

		SharedLibrary			  loader;
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
		INSTANCE_NULL_FUNC(vkCreateInstance) // These three functions are only used for creating a VkInstance.
		INSTANCE_NULL_FUNC(vkEnumerateInstanceExtensionProperties)
		INSTANCE_NULL_FUNC(vkEnumerateInstanceLayerProperties)
		std::unique_ptr<InstanceApiTable> api;
		UniqueVkInstance				  instance;
		UniqueVkDebugUtilsMessengerEXT	  messenger;

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
}
